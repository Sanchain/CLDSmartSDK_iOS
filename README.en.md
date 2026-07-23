# CLDSmartSDK_iOS

CLDSmartSDK for iOS provides account authentication, device binding, Bluetooth communication, IoT control, push messaging, and audio/video capabilities.

- Current version: `1.1.0`
- Minimum deployment target: iOS 13.0
- Swift: 5.9 or later
- Distribution: static XCFramework
- Full API documentation: [CLDSmartSDK Developer Documentation](https://wvue9d885o0.feishu.cn/wiki/FKAcwoh0TibL0Sk99nfcgD2gn8f)

> Account APIs in `1.1.0` are exposed to Swift. An Objective-C application must add a Swift wrapper before calling them.

## Installation

Add the SDK to the application's `Podfile`:

```ruby
platform :ios, '13.0'

source 'https://github.com/CocoaPods/Specs.git'

target 'YourApp' do
  use_frameworks!

  pod 'CLDSmartSDK_iOS',
      :git => 'https://github.com/Sanchain/CLDSmartSDK_iOS.git',
      :tag => '1.1.0'
end
```

Install dependencies:

```bash
pod install --repo-update
```

Open the generated `.xcworkspace`. CocoaPods also resolves VLink, CocoaMQTT, and Agora dependencies. Do not manually embed an older CLDSmartSDK, VLink, or CocoaMQTT binary because doing so can cause duplicate classes or symbols.

Import the SDK in Swift:

```swift
import CLDSmartSDK
```

Import VLink when directly using types such as `APBLEDevice`:

```swift
import VLink
```

## Initialization

All account and device APIs require a successful `initEngine` call. The SDK provider issues the App ID and Secret Key. Do not commit these credentials to a public repository or print them in logs.

```swift
import CLDSmartSDK

let engine = CldSmartEngine.shared

let config = CldSmartEngineConfig(
    serverCode: .us,
    language: .en,
    isDevServer: false
)

engine.initEngine(
    appId: "<YOUR_APP_ID>",
    // secertKey is the spelling used by the public 1.1.0 API.
    secertKey: "<YOUR_SECRET_KEY>",
    config: config
) { success in
    guard success else {
        print("CLDSmartSDK initialization failed")
        return
    }

    // Continue with one of the account flows below.
}
```

Server and account-region mapping:

| `serverCode` | Region | Account `countryCode` | `regionCode` |
| --- | --- | --- | --- |
| `.mainland` | Mainland China | `+86` | `CN` |
| `.us` | US/international email | `N` | `US` |
| `.taiwan` | Taiwan | `+886` | `TW` |

Set `isDevServer` to `true` for the test environment and `false` for production. The App ID, Secret Key, server region, and environment must belong to the same backend configuration.

## Account Modes

The SDK supports two mutually exclusive account integration modes. Select the mode agreed for the application and do not mix them in one user session.

### Mode 1: Customer-Owned Account

The customer application performs its own registration and login, obtains a stable and unique customer user ID, and then initializes the SDK session with `login(account:)`. This method is not a password-login API.

Required sequence:

```text
initEngine -> customer login -> stable customer user ID -> login(account:)
```

```swift
func loginWithCustomerAccount(
    customerUserID: String,
    completion: @escaping (Bool) -> Void
) {
    engine.initEngine(
        appId: "<YOUR_APP_ID>",
        secertKey: "<YOUR_SECRET_KEY>",
        config: config
    ) { initialized in
        guard initialized else {
            completion(false)
            return
        }

        // customerUserID must remain stable and unique in the customer system.
        engine.login(account: customerUserID) { success in
            if success {
                engine.getMqttConfig()
            }
            completion(success)
        }
    }
}
```

When switching users, clear the previous SDK session to prevent access to the previous user's device data:

```swift
engine.logout { _ in
    // Clear the local session even if server logout fails due to the network.
    engine.deinitEngine()

    engine.initEngine(
        appId: "<YOUR_APP_ID>",
        secertKey: "<YOUR_SECRET_KEY>",
        config: config
    ) { initialized in
        guard initialized else { return }

        engine.login(account: newCustomerUserID) { success in
            if success {
                engine.getMqttConfig()
            }
        }
    }
}
```

### Mode 2: CLDSmart Account

The customer uses SDK APIs for registration, password login, password recovery, and password changes. A successful `initEngine` call is still required before every flow.

Create the account context:

```swift
let authContext = CLDAuthContext(
    countryCode: "N",       // Use "+86" for a mainland China phone number.
    regionCode: "US",       // Use "CN" for mainland China.
    deviceToken: apnsToken,  // Hex-encoded APNs token.
    isAPNsSandbox: true      // true for APNs Sandbox, false for Production.
)
```

#### Registration

Registration has two steps: request a verification code and validate it.

```swift
engine.register(
    account: account,
    password: password,
    context: authContext
) { success, code, message in
    guard success, code == 20000 else {
        print(message ?? "Register failed")
        return
    }

    // Ask the user for the received verification code.
}

engine.validateRegistrationCode(
    account: account,
    code: verificationCode
) { success, code, message in
    if success, code == 20000 {
        // Registration is complete. Continue with password login.
    }
}
```

Passwords must contain 8 to 12 characters and at least two of these categories: letters, numbers, and symbols. Validate input before sending a request.

#### Password Login

```swift
engine.login(
    account: account,
    password: password,
    context: authContext
) { userInfo, code, message in
    guard code == 20000, let userInfo else {
        print(message ?? "Login failed")
        return
    }

    print("Logged in account: \(userInfo.account)")

    // Login and MQTT startup are separate operations.
    engine.getMqttConfig()
}
```

The SDK persists the access token and refresh token returned by password login. When the access token expires, the SDK coalesces concurrent refresh attempts and retries waiting requests after a successful refresh. Applications do not call `refresh-access-token` directly.

#### Password Recovery

Password recovery has three steps. `resetToken` is sensitive and should only be retained in memory for the current flow. Do not log or persist it.

```swift
engine.requestPasswordResetCode(
    account: account,
    context: authContext
) { success, code, message in
    // Ask the user for the received verification code.
}

engine.validatePasswordResetCode(
    account: account,
    code: verificationCode
) { resetToken, code, message in
    guard code == 20000, let resetToken, !resetToken.isEmpty else {
        return
    }

    engine.resetPassword(
        resetToken: resetToken,
        newPassword: newPassword,
        passwordConfirmation: newPassword
    ) { success, code, message in
        // Handle the result, then immediately discard resetToken.
    }
}
```

#### Set or Change Password

`setPassword` is only for an authenticated social account where `userInfo.has_password == false`.

```swift
engine.setPassword(password: newPassword) { success, code, message in
    // Handle the initial password setup result.
}

engine.changePassword(
    currentPassword: currentPassword,
    newPassword: newPassword,
    passwordConfirmation: newPassword
) { success, code, message in
    // Handle the password change result.
}
```

#### Logout

```swift
engine.logout { success in
    if !success {
        // A normal retry may retain the session. A user switch must also call
        // deinitEngine to remove the previous local session.
    }
}
```

## APNs Device Token

A production application must enable the Push Notifications capability and register for remote notifications. The login request uses the same APNs token for `device_token` and `reg_token`.

```swift
private var apnsToken = ""

func application(
    _ application: UIApplication,
    didRegisterForRemoteNotificationsWithDeviceToken deviceToken: Data
) {
    let token = deviceToken
        .map { String(format: "%02x", $0) }
        .joined()

    apnsToken = token

    // Update the backend when the token arrives after login.
    if CldSmartEngine.shared.isLoggedIn {
        CldSmartEngine.shared.refreshAPNs(token: token) { success in
            print("APNs token updated: \(success)")
        }
    }
}
```

Use `isAPNsSandbox: true` for Debug/Sandbox builds and `false` for the production APNs environment. The SDK generates a development placeholder when the token is empty, but a placeholder cannot receive notifications. Always provide a real token in production.

## Permissions and Capabilities

Configure only the permissions required by enabled features:

```xml
<key>NSBluetoothAlwaysUsageDescription</key>
<string>Used to discover and communicate with smart devices.</string>
<key>NSLocalNetworkUsageDescription</key>
<string>Used to discover and configure devices on the local network.</string>
<key>NSLocationWhenInUseUsageDescription</key>
<string>Used to read Wi-Fi information during device setup.</string>
<key>NSMicrophoneUsageDescription</key>
<string>Used for device voice intercom.</string>
```

- Bluetooth devices require Bluetooth permission.
- Wi-Fi provisioning that reads the SSID requires the Access WiFi Information capability and location permission.
- Push notifications require the Push Notifications capability. Enable the Remote notifications background mode only when needed.
- Voice intercom requires microphone permission.

Customers must adapt permission descriptions to the actual application behavior and privacy policy.

## Results and Threads

- The backend success code is `20000`.
- The common SDK network error is `CldSmartEngine.networkAnomalyCode`, currently `-1009`.
- Callbacks are not guaranteed to execute on the main thread. Dispatch UI updates to the main queue.
- Never log passwords, access tokens, refresh tokens, APNs tokens, or MQTT credentials.

## Troubleshooting

### `No such module CLDSmartSDK`

Run `pod install --repo-update` again and open the `.xcworkspace`.

### `APBLEResponse is implemented in both ...`

The application has loaded two VLink/APBLE implementations. Remove manually embedded legacy CLDSmartSDK, VLink, or duplicate Pods, clean the build folder, and reinstall the application.

### Login succeeds but MQTT does not connect

Call `engine.getMqttConfig()` after login succeeds. Login does not start MQTT automatically.

### `reg_token is a required field`

Obtain a real APNs token and pass it through `CLDAuthContext.deviceToken` before password login. `device_token` and `reg_token` must use the same non-empty value.

### Phone or email registration fails

Verify that the SDK server, App ID/Secret Key, `countryCode`, and `regionCode` belong to the same region. A mainland China phone number requires `countryCode: "+86"`; do not use `"86"` or `"N"`.

### A new account still sees the previous user's devices

Do not overwrite an existing session. Switch users in this order: server logout, `deinitEngine`, `initEngine`, and then new-user login.

## Release Notes

### 1.1.0

- Added registration, password login, password recovery, password setup, and password change APIs.
- Added concurrent access-token refresh and automatic original-request retry.
- Added both customer-owned and CLDSmart account integration modes.
- Changed the XCFramework to static distribution with Swift Module Interface files.
- Redacted sensitive MQTT connection data from logs.
