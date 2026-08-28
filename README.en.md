# CLDSmartSDK_iOS

CLDSmartSDK for iOS provides account authentication, device binding, Bluetooth communication, IoT control, push messaging, and audio/video capabilities.

- Current version: `1.4.17`
- Minimum deployment target: iOS 13.0
- Swift: 5.9 or later
- Distribution: static XCFramework
- Full API documentation: [CLDSmartSDK Developer Documentation](https://wvue9d885o0.feishu.cn/wiki/FKAcwoh0TibL0Sk99nfcgD2gn8f)

> Account authentication and account-deletion APIs in `1.2.0` are exposed to Swift. An Objective-C application must add a Swift wrapper before calling them.

## Installation

Add the SDK to the application's `Podfile`:

```ruby
platform :ios, '13.0'

source 'https://github.com/CocoaPods/Specs.git'

target 'YourApp' do
  use_frameworks!

  pod 'CLDSmartSDK_iOS',
      :git => 'https://github.com/Sanchain/CLDSmartSDK_iOS.git',
      :tag => '1.4.17'
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
    // secertKey is the spelling used by the public API.
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
| `.mainland` | Mainland China | Phone: `+86`; email: `N` | `CN` |
| `.us` | US/international email | `N` | `US` |
| `.taiwan` | Taiwan | Phone: `+886`; email: `N` | `TW` |

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
    countryCode: "N",       // Use "N" for email (including China); use "+86" for a mainland China phone.
    regionCode: "US",       // Use "CN" for any mainland China account.
    deviceToken: apnsToken,  // Pass an empty string if the APNs token is not available yet.
    isAPNsSandbox: true      // Match aps-environment; independent of the business server.
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

#### Delete a CLDSmart Account

Account deletion is only available to a user signed in through the CLDSmart password-login API. It has two steps: send a verification code, then confirm deletion. Deletion cannot be undone; a production application should ask for explicit confirmation before making the final request.

```swift
// ===== CLDSmart account-deletion example starts here =====

// Step 1: Send a deletion verification code to the current account.
func requestAccountDeletionCode() {
    engine.sendDeleteAccountVerificationCode { success, code, message in
        guard success, code == 20000 else {
            print(message ?? "Failed to send the account-deletion code")
            return
        }

        // Ask the user for the received code. Never print the code in logs.
    }
}

// Step 2: After confirmation, permanently delete the account with its
// current password and the received verification code.
func confirmAccountDeletion(password: String, verificationCode: String) {
    engine.deleteAccount(
        password: password,
        verificationCode: verificationCode
    ) { success, code, message in
        guard success, code == 20000 else {
            print(message ?? "Failed to delete the account")
            return
        }

        // The SDK has cleared local tokens and user data and disconnected MQTT.
        // Clear application-owned account UI state and return to the login screen.
    }
}

// ===== CLDSmart account-deletion example ends here =====
```

The existing `deleteAccount(captcha:)` overload deletes an OAuth account using an image captcha. Do not use it for the CLDSmart password-login flow.

#### Logout

```swift
engine.logout { success in
    if !success {
        // A normal retry may retain the session. A user switch must also call
        // deinitEngine to remove the previous local session.
    }
}
```

## Shared Members and Device Ownership Transfer

`updateShareMemberList` replaces the complete non-empty shared-member list for a device. It does not accept an incremental add/delete list. For backward compatibility, starting with `1.4.5`, an empty array returns `false` locally and does not send a backend request.

Use the explicit API to remove one member:

```swift
engine.removeShareMember(vid: deviceVID, memberId: targetMemberID) { success in
    guard success else {
        print("Failed to remove the shared member")
        return
    }
}
```

Require user confirmation before explicitly clearing all shared members:

```swift
engine.clearShareMemberList(vid: deviceVID) { success in
    guard success else {
        print("Failed to clear shared members")
        return
    }
}
```

All three methods reuse the existing full-list replacement backend route; no additional backend route is required. `removeShareMember` first fetches the current list and then submits the remaining complete list, including an empty list when removing the last member. Starting with `1.4.6`, the SDK serializes add, remove, and clear operations by VID within the current application process; different VIDs can still run concurrently. The application UI should still prevent duplicate submissions. A local queue cannot prevent full-list overwrites when multiple phones or application processes modify the same VID; complete protection requires backend version checks or atomic add/remove routes.

To transfer device ownership, first call `fetchChangeOwnerCode` and provide at least one of `account` or `identify`. Receiving an `event_code` only completes the account-confirmation step. The application must still complete the captcha flow and call `verifyChangeOwner`.

```swift
engine.fetchChangeOwnerCode(
    vid: deviceVID,
    account: targetAccount
) { eventCode in
    guard let eventCode else {
        print("Failed to obtain the ownership-transfer code")
        return
    }

    // Obtain and display the captcha, then pass the user's input here.
    engine.verifyChangeOwner(
        code: eventCode,
        captcha: captcha
    ) { success in
        print("Ownership transfer result: \(success)")
    }
}
```

Version `1.4.4` fixes the missing `devices/` path segment that caused the account-confirmation request to return HTTP 404.

## Delete a Device

Version `1.3.0` provides one device-deletion API for both account modes. The SDK selects the matching server API from the current login mode, so the application does not determine the account type itself.

- Customer-owned account via `login(account:)`: automatically uses image-captcha device deletion.
- CLDSmart password login via `login(account:password:context:)`: automatically generates the signature and uses signed device deletion.
- `reset: true`: unbinds the device and clears its data. Use this for the normal confirmed deletion flow.
- `reset: false`: only unbinds the device and preserves its data.

```swift
// ===== Unified device-deletion example starts here =====

func deleteDevice(vid: String) {
    // A production application should show an irreversible-action confirmation first.
    engine.deleteDevice(
        vid: vid,
        reset: true
    ) { success, code, message in
        guard success, code == 20000 else {
            print(message ?? "Failed to delete the device")
            return
        }

        // Refresh the device list and close the current device-details screen.
    }
}

// ===== Unified device-deletion example ends here =====
```

Sessions cached by `1.2.0` or earlier do not contain a login mode. After upgrading, have the user log out and sign in again before the first unified device-deletion call. Otherwise, the API returns `40101` with `Unknown login mode. Please log in again.` to prevent the SDK from selecting the wrong deletion API.

The existing `deleteDevice(vid:code:token:completion:)` API remains available for source compatibility. New integrations should use `deleteDevice(vid:reset:completion:)`.

## BLE Connection and Remote Lock Concurrency

Starting with `1.4.6`, `connectBLE(name:config:encryptKey:timeout:completion:)` and `connectBLE(device:config:encryptKey:timeout:completion:)` share one global connection coordinator. Both completions run on the main thread at most once. VLink uses a global BLE manager, so the two entry points accept only one combined connection attempt at a time; an overlapping call immediately returns `false` without canceling the active attempt. Completion cancels the matching SDK fallback timeout. After a timeout, wait for completion before retrying so the SDK can clean up the old connection before a new attempt starts. The device-based entry point now honors the caller-provided `timeout` and safely accepts a `nil` completion.

The completion of `unlockDevice(vid:unLock:timeout:completion:)` also runs on the main thread at most once. Observers, timers, and task state are isolated by VID, so different VIDs can run concurrently. A same-VID overlap immediately returns `false` without interrupting the active operation. The status timeout starts after backend acceptance, preserving the legacy timing semantics.

## Fingerprint Enrollment and List Synchronization

Run the fingerprint flow in this order: BLE ready, enrollment success, local-list upload, then cloud-list query. `bleIsConnected` only indicates a physical connection. Verify `bleIsReady == true` before sending a command. The device may report `progress == 100` while the state is still `.inputting`; only `.inputSuccess` is final success.

```swift
func enrollFingerprint(device: CLDDevice, name: String) {
    let engine = CldSmartEngine.shared
    let fingerprint = CLDPassword(
        unlock_type: CLDInputKeyType.fingerprint.rawValue,
        note: name
    )

    guard engine.bleIsReady else {
        print("The VLink handshake is not ready")
        return
    }

    engine.addKeyInput(
        vid: device.vid,
        password: fingerprint,
        isBLE: true,
        timeout: max(device.input_key_timeout.fingerprint, 120)
    ) { progress, state in
        print("fingerprint: progress=\(progress), state=\(state.rawValue)")
        guard state == .inputSuccess else { return }

        engine.syncDeviceKeyListFromBLE(
            vid: device.vid,
            type: .fingerprint
        ) { success, localCount, code, message in
            guard success, code == 20000 else {
                print(message ?? "Failed to synchronize device fingerprints")
                return
            }

            engine.getDeviceKeyList(
                vid: device.vid,
                type: .fingerprint
            ) { list, code, message in
                guard code == 20000, let list else {
                    print(message ?? "Failed to query cloud fingerprints")
                    return
                }
                print("Local fingerprints: \(localCount), cloud fingerprints: \(list.count)")
            }
        }
    }
}
```

When the device is not connected, first call `connectBLE` with its `bluetooth_id`, `input_bt_config ?? bt_config`, and `bt_secret`; verify `bleIsReady` after the success callback. Do not start another connection while one is connected or connecting. `getDeviceKeyList` queries cloud records and does not read the lock directly, so call `syncDeviceKeyListFromBLE` first after BLE enrollment. Never log `bt_secret`, tokens, or raw encrypted BLE data.

## Bluetooth-Only Device OTA

DL500 (`match_num == 11`), Keypad (`13`), KeypadP (`74`), and other devices whose primary `bt_config.protocol == 1` and firmware supports C010-C014 use `upgradeBLEDevice`. WiFi/networked devices continue to use MQTT through `upgradeDevice`. Do not select OTA transport from `match_num`.

```swift
let engine = CldSmartEngine.shared

guard device.bt_config.protocol == 1 else {
    // Call upgradeDevice for a WiFi/networked device.
    return
}

engine.upgradeBLEDevice(vid: device.vid, timeout: 5) {
    progress, status, errorCode, message in
    switch status {
    case .success:
        // success is emitted only after the device accepts C014 validation.
        print("BLE OTA success")
    case .failed:
        print("BLE OTA failed: \(errorCode ?? 0), \(message ?? "")")
    case .cancelled:
        print("BLE OTA cancelled")
    default:
        print("BLE OTA \(status): \(progress)%")
    }
}
```

The SDK resolves device and firmware metadata from `vid`, verifies that the primary `bt_config.protocol == 1`, downloads the firmware, connects to the target BLE device when needed, and executes C010 through C014. Download progress maps to `0...49`, BLE transfer maps to `50...99`, and successful C014 validation emits `100`.

- `.success` is the only successful terminal state. It means C014 did not time out and `response.data[1] == 0`.
- `.verifying` is still in progress and must not be shown as success.
- Cloud OTA status reporting records the result but does not determine device success.
- Only one BLE OTA can run at a time. Use `bleOTAIsRunning` to prevent duplicate starts.
- Call `cancelBLEOTA()` for an explicit user cancellation; the original callback receives `.cancelled`.
- Keep the app in the foreground and the device awake and powered. Do not send other BLE commands during OTA.

Starting with `1.4.11`, filter field logs by `[CLDSmartSDK][BLEOTA]`. The logs include an operation ID, elapsed time, firmware size, negotiated packet length, packet index/count, retry count, latest RTT and response, BLE state, and the terminal error. Firmware bytes, Bluetooth secrets, and account credentials are never logged. With the default `timeout=5`, an unchanged C013 packet fails with `.failed` and error `-5112` after approximately 15 seconds instead of remaining in transfer indefinitely.

The Keypad/KeypadP scan result `binded` comes directly from byte 4 of manufacturer data; it is not read from an SDK cache or the cloud device list. For example, byte 4 is `01` in `FFFF010100000D`, so the device is still advertising itself as bound. A fully reset device should advertise `FFFF010000000D`. If `01` remains after a reset, verify that a full factory reset was performed, advertising was restarted, and the device firmware cleared its local binding flag.

The `1.4.9` `upgradeDL500Device`, `cancelDL500OTA`, `dl500OTAIsRunning`, and legacy DL500 types still compile, but are deprecated wrappers around the generic BLE OTA implementation. WiFi/networked devices continue to use `upgradeDevice(vid:timeout:completion:)` without behavior changes.

## APNs Device Token

The app must enable the Push Notifications capability and register for remote notifications.
The login request uses the same APNs token for `device_token` and `reg_token`. Because the
token can arrive before or after login completes, cache it first and synchronize it only after
both `initEngine` and password login succeed.

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

    // Update the backend immediately after login; cache it before login.
    if CldSmartEngine.shared.isLoggedIn {
        CldSmartEngine.shared.refreshAPNs(token: token) { success in
            print("APNs token updated: \(success)")
        }
    }
}
```

If the token was already cached, call `refreshAPNs(token:completion:)` once after password login
succeeds. Set `isAPNsSandbox` from the installed app's signed `aps-environment` entitlement:
use `true` for `development` and `false` for `production`. This setting is independent of the
`.us` region and `isDevServer` business-server selection. The SDK creates a unique placeholder
when the token is empty, but a placeholder cannot receive notifications.

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

Verify that the SDK server, App ID/Secret Key, `countryCode`, and `regionCode` belong to the same region. A mainland China phone number uses `countryCode: "+86"` and `regionCode: "CN"`; a mainland China email account uses `countryCode: "N"` and `regionCode: "CN"`.

### A new account still sees the previous user's devices

Do not overwrite an existing session. Switch users in this order: server logout, `deinitEngine`, `initEngine`, and then new-user login.

## Release Notes

### 1.4.17

- Fixed Release SDK builds selecting APNs Production when refreshing a Sandbox device token. A successful password login now persists `CLDAuthContext.isAPNsSandbox`, and later token refreshes reuse that environment.
- `isAPNsSandbox` now explicitly follows the app's signed `aps-environment` entitlement and is independent of the test/production business server. `ios_dev=1` selects Sandbox and `ios_dev=0` selects Production.
- Login and refresh requests remove angle brackets and whitespace from APNs tokens. Diagnostic logs contain only the environment, `ios_dev`, and token length, never the token value.
- Public API signatures are unchanged. Legacy `login(account:)` and sessions cached by older SDKs retain their previous default behavior. Password-login clients must log in once after upgrading and then synchronize the APNs token.
- Device arm64 and simulator arm64/x86_64 XCFrameworks, Swift interfaces, CocoaPods lint, and a temporary Release client build passed. Final acceptance still requires a physical-device push and an Apple APNs HTTP 200 response from the selected environment.

### 1.4.16

- Added Google Assistant and Alexa cloud authorization APIs: `getVoiceAssistantAvailability`, `getVoiceAssistantLinkStatus`, `getAlexaAuthorizationLinks`, `completeAlexaLink`, `authorizeAlexaSkill`, `getGoogleAssistantAuthorizationInfo`, `unlinkVoiceAssistant`, and `setGoogleAssistantPIN`.
- Added public models `CLDVoiceAssistant`, `CLDVoiceAssistantAvailability`, `CLDVoiceAssistantLinkStatus`, `CLDAlexaAuthorizationLinks`, `CLDAlexaSkillAuthorizationRequest`, `CLDAlexaSkillAuthorizationResult`, and `CLDGoogleAssistantAuthorizationInfo`.
- SDK logs redact OAuth authorization codes, `state`, authorization URLs, deep links, and voice PINs. Client apps remain responsible for opening authorization entries, handling universal links, and completing the flow with customer-specific client IDs, Alexa skills, Google actions, and real accounts.
- UI automation covers the Voice Assistants demo page, its nine input fields, and ten API actions. Device arm64 and simulator arm64/x86_64 XCFramework slices, Swift interfaces, CocoaPods lint, and a temporary client project build also passed validation.
- Simulator automation without third-party accounts does not cover the real Alexa/Google OAuth lifecycle. Authorization, callbacks, link status, unlinking, and Google PIN must still be validated before production integration.

### 1.4.15

- Added `CLDSessionInvalidReason`, `sessionInvalidHandler`, `lastSessionInvalidReason`, and `Notification.Name.cldSessionDidInvalidate` so client apps can handle terminal sessions that require a new login.
- For `40101`, the SDK first refreshes the access token and only invalidates the session if refresh or retry fails. `40102` means the account logged in on another client, and `40105` means the refresh token expired; both invalidate the session immediately.
- Session-invalid callbacks run on the main thread after local user data is cleared and MQTT is disconnected. Subsequent public API calls preserve the latest terminal error code.
- Fixed duplicate invalidation events from concurrent requests and delayed responses from a previous account clearing a newer session.
- `markNotificationAsRead(notifyId:completion:)` and `deleteNotifications(notifyIds:completion:)` remain available for notification management.
- Device arm64 and simulator arm64/x86_64 XCFramework slices, Swift interfaces, SDK/Demo builds, and the CocoaPods temporary client project passed validation.

### 1.4.14

- Added `markNotificationAsRead(notifyId:completion:)` to mark one notification from `CLDEvent.notify_id` as read.
- Added `deleteNotifications(notifyIds:completion:)` to delete 1 to 100 notifications per request. Deletion is irreversible, so client applications should request confirmation first.
- Both APIs return `success/code/message`; invalid IDs return local code `-1000` without sending a network request.
- The Demo Events page and customer API catalog now include runnable examples, input validation, and delete confirmation.
- Device arm64 and simulator arm64/x86_64 XCFramework slices, Swift interfaces, and the CocoaPods temporary client project passed validation.

### 1.4.13

- Added `requiresLatchCalibration`, `isMagneticCalibrated`, and `isCalibrated` to `CLDLockCalibrationResult`, so customer pages do not interpret raw `0x9077` bytes.
- Clarified that `magneticType == 1` means the door sensor is calibrated; corrected and deprecated the ambiguous `magneticNeedsCalibration` property.
- Calibration sessions now select an internal protocol adapter by `CLDLockCalibrationProfile.identifier`. Unknown profiles return `unsupported` instead of sending the standard lock protocol.
- The Demo emergency-unlock completion path now calls `fetchLockCalibrationStatus` instead of sending and decoding `0x9077` itself.
- Device arm64 and simulator arm64/x86_64 XCFramework slices, Swift interfaces, SDK and Demo builds, and the CocoaPods temporary client project passed validation.

### 1.4.12

- Added the model-independent `CLDLockCalibrationSession` for manual/automatic calibration, latch and entry selection, door sensor setup, unlock/lock tests, cancellation, and completion.
- Added typed `CLDLockCalibrationStage`, `CLDLockCalibrationEvent`, `CLDLockCalibrationError`, and `CLDLockCalibrationResult` so customer pages consume business results instead of parsing BLE `Data`.
- `complete` exits calibration and reads `latchType`, `magneticType`, and `state`; after emergency unlock or another lock workflow, `fetchFinalStatus` reads the same typed result.
- Added `CLDLockCalibrationProfile` as the future protocol-adapter entry point without model-specific public API names.
- Device arm64, simulator arm64/x86_64 XCFramework, Swift interfaces, and the Demo customer-page build passed validation.

### 1.4.11

- Fixed a `1.4.10` race where consecutive C013 packets could lose the next callback and timer, leaving OTA permanently in `.transferring`. Client call sites do not change.
- Added `[CLDSmartSDK][BLEOTA]` structured diagnostics for download, connection, C010-C014, packet progress, retries, RTT, responses, BLE state, and terminal errors.
- Added an independent C013 no-progress watchdog. With the default `timeout=5`, approximately 15 seconds without progress returns `.failed/-5112`.
- Renamed the implementation file to `CldEngineBluetoothOTAEx.swift` to reflect support for every Bluetooth-only device with the required transport configuration and OTA protocol. Scanning, connection, and generic commands remain in `CldEngineBluetoothEx.swift`.
- Documented the Keypad/KeypadP manufacturer-data binding flag: `FFFF010100000D` advertises bound, while `FFFF010000000D` advertises unbound.
- Device arm64 and simulator arm64/x86_64 XCFrameworks, Swift interfaces, CocoaPods lint, and client builds passed. Physical-device OTA acceptance is still required for DL500, Keypad, and KeypadP firmware.

### 1.4.10

- Generalized the DL500-specific OTA flow for Bluetooth-only devices. DL500 (`match_num == 11`), Keypad (`13`), KeypadP (`74`), and other devices whose primary `bt_config.protocol == 1` and firmware supports C010-C014 now use `upgradeBLEDevice(vid:timeout:completion:)`.
- Added `CLDBLEOTAStatus`, `CLDBLEOTAErrorCode`, `bleOTAIsRunning`, and `cancelBLEOTA()`. The legacy DL500 APIs and types remain available as deprecated compatibility wrappers.
- OTA transport selection is no longer hard-coded by model. Primary `bt_config.protocol == 1` uses BLE C010-C014; WiFi/networked devices continue to use MQTT through `upgradeDevice`.
- Device upgrade success strictly requires a non-timeout C014 response with at least two bytes and `response.data[1] == 0`. Download completion, C013 completion, 99% progress, or successful cloud reporting do not indicate device success.
- The device arm64 and simulator arm64/x86_64 XCFramework slices passed validation and a CocoaPods temporary client project compiled successfully. Target firmware still requires physical-device acceptance on DL500, Keypad, and KeypadP before production rollout.

### 1.4.9

- Added `upgradeDL500Device(vid:timeout:completion:)` to orchestrate DL500 firmware lookup, HTTP download, target BLE connection, and the complete C010-C014 OTA protocol.
- Added `CLDDL500OTAStatus`, `CLDDL500OTAErrorCode`, `dl500OTAIsRunning`, and `cancelDL500OTA()`. Progress and terminal callbacks are delivered on the main thread.
- DL500 success now strictly requires C014 whole-file validation result 0. Download completion, C013 completion, and successful cloud status reporting are not device-upgrade success.
- Added single-operation exclusion, response-length checks, first/last-packet bounds handling, one independent timeout retry per packet, and terminal handling for disconnect, background entry, and cancellation.
- Existing MQTT OTA behavior and `upgradeDevice` signatures for WiFi/networked devices remain unchanged.

### 1.4.8

- Routed accessory binding and unbinding by their actual transport. `bindWiFiAccessory` and `unbindWiFiAccessory` preserve the existing WiFi backend payload and omit `is_blue`; BLE accessory APIs add `is_blue=1` internally.
- The Keypad/DL500 set uses a BLE accessory relationship: Keypad is the primary device and DL500 is the accessory. Other WiFi accessories continue to use the WiFi APIs and the existing MQTT confirmation flow.
- Client applications do not need to construct `is_blue` and should not do so. Existing public method signatures are unchanged, and clients that do not use accessory APIs are unaffected.
- The device arm64 and simulator arm64/x86_64 XCFramework slices passed structural validation and CocoaPods temporary-project compilation.

### 1.4.7

- Added optional `CLDDeviceInfo.is_online` and `CLDDeviceInfo.power` for the cloud online state and power snapshot returned by the device-info API. Missing fields decode as `nil`, and `power_status` keeps its existing semantics.
- Added `formatSDCard(vid:completion:)`. A successful HTTP response only means the request was accepted; final success requires the MQTT `stored_reset` event with `params.result == 1`.
- Fixed the `MediaCore` recorder lifecycle so stop, failure, and destroy release the recorder cleanly and allow recording or live-view reconstruction. Public method signatures are unchanged.
- Started the existing OTA timeout timer immediately after the request succeeds and MQTT subscription is installed.
- Repaired the simulator `CLDSmartSDK-Swift.h`; device arm64 and simulator arm64/x86_64 all pass Clang validation.
- No existing API was removed or re-signed. Existing clients only need to update the complete XCFramework/Pod and rebuild. Code that exhaustively switches over `NetworkAPI` must add `.resetSDCard` or `@unknown default`.

### 1.4.6

- Serialized shared-member updates, single-member removal, and full clearing by VID within the current application process, while allowing different VIDs to proceed concurrently.
- Unified both `connectBLE` overloads under one global coordinator; overlapping calls return `false`, each attempt has an independent ID and cancelable timeout, and completion runs on the main thread at most once.
- Updated `connectBLE(device:...)` to honor the caller-provided `timeout` and safely support a `nil` completion.
- Kept the public Swift Interface and Objective-C header identical to `1.4.5`, so customer method signatures do not change.

### 1.4.5

- Restored the legacy empty-array guard in `updateShareMemberList`; an empty array returns `false` locally and no longer clears members implicitly.
- Added `removeShareMember` and `clearShareMemberList` for explicit single-member removal and full clearing while reusing the existing backend full-list route.
- Added per-attempt IDs, cancelable timeout work, and a disconnect cleanup window to BLE name-based connections; completion runs on the main thread at most once, and overlapping calls do not interrupt the active attempt.
- Isolated remote lock observers, timers, and state by VID; different VIDs can run concurrently, same-VID overlap returns `false`, and stale tasks cannot complete newer tasks.

### 1.4.4

- Fixed the device ownership account-confirmation route so `fetchChangeOwnerCode` no longer receives HTTP 404.
- Fixed deletion of the final shared member; `updateShareMemberList` now accepts an empty array to clear all shared members.
- Documented full-list replacement semantics for shared members and the device ownership transfer flow.

### 1.4.3

- Fixed fingerprint-list requests so `.fingerprint` sends the backend value `finger_print`.
- Added `bleIsReady` to distinguish a physical BLE connection from a connection with a completed VLink random-code/key handshake.
- Added `syncDeviceKeyListFromBLE` to read local lock credentials and upload them before querying the cloud list.
- Fixed BLE fingerprint progress forwarding and stale busy-state handling; `progress=100` is no longer treated as final success by itself.
- Linked CLDSmartSDK and VLink statically to prevent duplicate Objective-C class implementations in host applications.

### 1.4.2

- Added the optional mutable `CLDPassword.is_duress_password` field for creating duress passwords.
- Set `password.is_duress_password = true` before calling `addDigitsPassword`; leaving it unset preserves the existing request payload.

### 1.4.1

- Added optional `CLDDeviceKey.is_duress_password` to identify duress passwords, fingerprints, palm-vein credentials, and other duress-marked keys.
- Accepts server values encoded as `true/false`, `0/1`, or the legacy `is_duress` field; returns `nil` when the server omits the flag.
- Guest passwords remain identified by `unlock_type == .tmpPassword`; regular user passwords use `unlock_type == .password`.

### 1.4.0

- Fixed `getDeviceKeyList` returning an empty array when temporary, periodic, or other password credentials were present.
- `CLDDeviceKey.unlock_type` now returns the detailed `CLDDeviceUnlockType`, distinguishing permanent, TOTP, temporary, periodic, and administrator passwords.
- Added `CLDDeviceKey.unlock_type_raw` so unknown future server values remain available without failing the entire page decode.
- Password-list requests now explicitly send `key_type=password`, and rows are decoded independently so one malformed record does not erase the complete list.
- Added numeric-password acceptance details and MQTT/network diagnostics to distinguish server acceptance, device execution, and list synchronization.

### 1.3.0

- Added `CLDLoginMode` to distinguish customer-owned sessions from CLDSmart password-login sessions.
- Added the unified `deleteDevice(vid:reset:completion:)` API.
- The SDK now selects image-captcha or signed device deletion from the current login mode.
- Updated the Demo device-details screen to use the unified API and display server error codes and messages.
- Preserved `deleteDevice(vid:code:token:completion:)` for source compatibility.

### 1.2.0

- Added `sendDeleteAccountVerificationCode(completion:)` for requesting a CLDSmart account-deletion code.
- Added `deleteAccount(password:verificationCode:completion:)` for deleting a CLDSmart account with its password and verification code.
- A successful deletion now clears SDK tokens and user data and disconnects MQTT.
- Preserved the existing OAuth `deleteAccount(captcha:)` API for source compatibility.

### 1.1.0

- Added registration, password login, password recovery, password setup, and password change APIs.
- Added concurrent access-token refresh and automatic original-request retry.
- Added both customer-owned and CLDSmart account integration modes.
- Changed the XCFramework to static distribution with Swift Module Interface files.
- Redacted sensitive MQTT connection data from logs.
