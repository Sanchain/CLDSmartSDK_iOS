# CLDSmartSDK_iOS

CLDSmartSDK iOS SDK 提供账号认证、设备绑定、蓝牙通信、IoT 控制、消息推送和音视频能力。

- 当前版本：`1.3.0`
- 最低系统：iOS 13.0
- Swift：5.9 或更高版本
- 分发形式：静态 XCFramework
- 完整接口文档：[CLDSmartSDK 开发文档](https://wvue9d885o0.feishu.cn/wiki/FKAcwoh0TibL0Sk99nfcgD2gn8f)

> `1.2.0` 的账号认证和账号删除接口面向 Swift。Objective-C 工程需要增加一层 Swift 包装后调用。

## 安装

在客户 App 的 `Podfile` 中添加：

```ruby
platform :ios, '13.0'

source 'https://github.com/CocoaPods/Specs.git'

target 'YourApp' do
  use_frameworks!

  pod 'CLDSmartSDK_iOS',
      :git => 'https://github.com/Sanchain/CLDSmartSDK_iOS.git',
      :tag => '1.3.0'
end
```

执行：

```bash
pod install --repo-update
```

请使用 `.xcworkspace` 打开工程。Pod 会同时处理 VLink、CocoaMQTT 和 Agora 依赖；不要再手动嵌入旧版 `CLDSmartSDK.xcframework`、VLink 或 CocoaMQTT，否则可能出现重复类或重复符号。

Swift 文件中导入：

```swift
import CLDSmartSDK
```

直接使用 `APBLEDevice` 等 VLink 类型时，还需要：

```swift
import VLink
```

## 初始化

SDK 的所有账号和设备接口都必须在 `initEngine` 成功后调用。App ID 和 Secret Key 由 SDK 服务方分配，不要提交到公开仓库或输出到日志。

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
    // secertKey 是公开 API 的实际参数名，请保持该拼写。
    secertKey: "<YOUR_SECRET_KEY>",
    config: config
) { success in
    guard success else {
        print("CLDSmartSDK initialization failed")
        return
    }

    // 在这里继续执行账号登录流程。
}
```

服务器配置：

| `serverCode` | 区域 | 账号 `countryCode` | `regionCode` |
| --- | --- | --- | --- |
| `.mainland` | 中国大陆 | 手机号：`+86`；邮箱：`N` | `CN` |
| `.us` | 美国/国际邮箱 | `N` | `US` |
| `.taiwan` | 中国台湾 | 手机号：`+886`；邮箱：`N` | `TW` |

`isDevServer: true` 使用测试环境，`false` 使用正式环境。App ID、Secret Key、服务器区域和环境必须属于同一套后台配置。

## 选择账号模式

SDK 支持两种互斥的账号接入模式。每个 App 应根据合作方案选择一种，不要在同一用户会话中混用。

### 模式一：客户自有账号

客户 App 自己完成注册和登录，取得稳定且唯一的客户用户 ID，然后调用 `login(account:)` 初始化 SDK 用户会话。该方法不是密码登录接口。

调用顺序：

```text
initEngine -> 客户 App 登录 -> 取得稳定用户 ID -> login(account:)
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

        // customerUserID 必须在客户账号系统内长期稳定且唯一。
        engine.login(account: customerUserID) { success in
            if success {
                engine.getMqttConfig()
            }
            completion(success)
        }
    }
}
```

切换客户用户时，必须清除旧用户会话，防止读取到上一个用户的设备数据：

```swift
engine.logout { _ in
    // 即使服务端登出因网络问题失败，也应清除本地旧会话。
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

### 模式二：CLDSmart 公司账号

客户直接使用 SDK 提供的注册、密码登录、找回密码和修改密码接口。所有接口仍然要求先成功调用 `initEngine`。

账号上下文示例：

```swift
let authContext = CLDAuthContext(
    countryCode: "N",       // 邮箱（包括中国区邮箱）使用 "N"；中国大陆手机号使用 "+86"
    regionCode: "US",       // 中国区账号（手机号或邮箱）均使用 "CN"
    deviceToken: apnsToken,  // APNs 十六进制 Token
    isAPNsSandbox: true      // APNs Sandbox 为 true，Production 为 false
)
```

#### 注册

注册分两步：发送验证码，然后校验验证码。

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

    // 提示用户输入收到的验证码。
}

engine.validateRegistrationCode(
    account: account,
    code: verificationCode
) { success, code, message in
    if success, code == 20000 {
        // 注册完成，下一步调用密码登录。
    }
}
```

密码规则：8 至 12 位，字母、数字、符号至少包含两类。App 应在请求前完成输入校验。

#### 密码登录

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

    // 登录和 MQTT 启动是两个独立步骤。
    engine.getMqttConfig()
}
```

SDK 会保存登录响应中的 access token 和 refresh token。密码登录会话的 access token 失效时，SDK 会合并并发刷新请求；刷新成功后自动重试等待中的原请求。客户 App 不需要直接调用 `refresh-access-token`。

#### 找回密码

找回密码分三步。`resetToken` 属于敏感信息，应只在本次流程的内存中短暂保存，不要写日志或持久化。

```swift
engine.requestPasswordResetCode(
    account: account,
    context: authContext
) { success, code, message in
    // 成功后提示用户输入验证码。
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
        // 处理重置结果，并立即清除内存中的 resetToken。
    }
}
```

#### 设置或修改密码

`setPassword` 仅适用于已经登录且 `userInfo.has_password == false` 的第三方登录账号。

```swift
engine.setPassword(password: newPassword) { success, code, message in
    // 处理首次设置密码结果。
}

engine.changePassword(
    currentPassword: currentPassword,
    newPassword: newPassword,
    passwordConfirmation: newPassword
) { success, code, message in
    // 处理修改密码结果。
}
```

#### 删除公司账号

删除账号仅适用于已通过公司账号密码接口登录的用户，分为“发送验证码”和“确认删除”两步。删除操作不可恢复，正式 App 应在调用前再次向用户确认。

```swift
// ===== 删除公司账号调用示例开始 =====

// 第一步：向当前登录账号发送删除验证码。
func requestAccountDeletionCode() {
    engine.sendDeleteAccountVerificationCode { success, code, message in
        guard success, code == 20000 else {
            print(message ?? "发送删除账号验证码失败")
            return
        }

        // 提示用户输入收到的验证码，不要在日志中打印验证码。
    }
}

// 第二步：用户确认后，使用当前密码和验证码永久删除账号。
func confirmAccountDeletion(password: String, verificationCode: String) {
    engine.deleteAccount(
        password: password,
        verificationCode: verificationCode
    ) { success, code, message in
        guard success, code == 20000 else {
            print(message ?? "删除账号失败")
            return
        }

        // SDK 已清除本地 Token、用户资料并断开 MQTT。
        // 客户 App 应清理自己的账号页面状态并返回登录页。
    }
}

// ===== 删除公司账号调用示例结束 =====
```

原有 `deleteAccount(captcha:)` 是 OAuth 账号的图片验证码删除接口，不要用于公司账号密码登录流程。

#### 登出

```swift
engine.logout { success in
    if !success {
        // 普通重试可以保留会话；切换用户时必须额外调用 deinitEngine 清理。
    }
}
```

## 删除设备

`1.3.0` 提供统一删除设备接口。客户自有账号和 CLDSmart 公司账号使用相同调用方式，SDK 会根据当前登录模式自动选择对应的服务端接口，客户 App 不需要自行判断账号类型。

- 客户自有账号 `login(account:)`：自动使用图片验证码删除接口。
- 公司账号密码登录 `login(account:password:context:)`：自动生成签名并使用签名删除接口。
- `reset: true`：解绑设备并清除设备数据，适用于用户确认删除设备的常规流程。
- `reset: false`：仅解绑设备，不清除设备数据。

```swift
// ===== 统一删除设备调用示例开始 =====

func deleteDevice(vid: String) {
    // 正式 App 应在调用前向用户显示不可恢复的删除确认提示。
    engine.deleteDevice(
        vid: vid,
        reset: true
    ) { success, code, message in
        guard success, code == 20000 else {
            print(message ?? "删除设备失败")
            return
        }

        // 删除成功后刷新设备列表，并关闭当前设备详情页。
    }
}

// ===== 统一删除设备调用示例结束 =====
```

从 `1.2.0` 或更早版本升级后，旧缓存会话不包含登录模式。首次调用统一删除设备接口前，应让用户退出并重新登录一次。否则接口会返回 `40101` 和 `Unknown login mode. Please log in again.`，用于防止 SDK 误用不匹配的删除接口。

原有 `deleteDevice(vid:code:token:completion:)` 继续保留以兼容已有客户代码。新接入项目应优先使用 `deleteDevice(vid:reset:completion:)`。

## APNs 推送 Token

正式 App 应启用 Push Notifications Capability，并注册远程通知。登录请求中的 `device_token` 和 `reg_token` 使用同一个 APNs Token。

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

    // 如果 Token 在登录后才取得，通知服务端更新。
    if CldSmartEngine.shared.isLoggedIn {
        CldSmartEngine.shared.refreshAPNs(token: token) { success in
            print("APNs token updated: \(success)")
        }
    }
}
```

Debug/Sandbox 构建使用 `isAPNsSandbox: true`，正式 APNs 环境使用 `false`。SDK 在 Token 为空时会生成开发占位值，但占位值不能接收推送，生产环境必须传入真实 Token。

## 权限与 Capability

根据实际启用的功能配置：

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

- 蓝牙设备：开启 Bluetooth 权限。
- Wi-Fi 配网需要读取 SSID 时：开启 Access WiFi Information Capability，并配置定位权限。
- 推送：开启 Push Notifications；需要后台接收时再开启 Remote notifications Background Mode。
- 视频对讲：使用语音功能时配置麦克风权限。

权限用途文案应由客户根据 App 的实际业务和隐私政策调整。

## 请求结果与线程

- 服务端业务成功码为 `20000`。
- SDK 网络异常公共错误码为 `CldSmartEngine.networkAnomalyCode`，当前值为 `-1009`。
- 回调不保证位于主线程；更新 UI 时请切换到主线程。
- 不要在日志中输出密码、access token、refresh token、APNs Token、MQTT 用户名或密码。

## 常见问题

### `No such module CLDSmartSDK`

重新执行 `pod install --repo-update`，并使用 `.xcworkspace` 打开工程。

### `APBLEResponse is implemented in both ...`

工程同时加载了两份 VLink/APBLE 实现。删除手动嵌入的旧版 CLDSmartSDK、VLink 或重复 Pod，清理 Build Folder 后重新安装 App。

### 登录成功但 MQTT 未连接

确认登录成功后调用了 `engine.getMqttConfig()`。登录 API 本身不会自动启动 MQTT。

### `reg_token is a required field`

生产环境应先取得真实 APNs Token，并通过 `CLDAuthContext.deviceToken` 传入密码登录接口。确认 `device_token` 和 `reg_token` 使用同一个非空值。

### 手机号或邮箱注册失败

确认 SDK 服务器、App ID/Secret Key、`countryCode` 和 `regionCode` 属于同一区域。中国大陆手机号使用 `countryCode: "+86"`、`regionCode: "CN"`；中国大陆邮箱使用 `countryCode: "N"`、`regionCode: "CN"`。

### 切换账号后仍看到旧设备

不要直接用新账号覆盖旧会话。按“服务端登出 -> `deinitEngine` -> `initEngine` -> 新账号登录”的顺序切换。

## 版本说明

### 1.3.0

- 增加登录模式 `CLDLoginMode`，区分客户自有账号和 CLDSmart 公司账号密码登录。
- 增加统一删除设备接口 `deleteDevice(vid:reset:completion:)`。
- SDK 根据登录模式自动选择图片验证码删除接口或签名删除接口。
- Demo 设备详情页改为使用统一删除设备接口，并显示服务端错误码和错误信息。
- 保留原有 `deleteDevice(vid:code:token:completion:)`，兼容已有客户代码。

### 1.2.0

- 增加公司账号删除验证码接口 `sendDeleteAccountVerificationCode(completion:)`。
- 增加公司账号密码与验证码删除接口 `deleteAccount(password:verificationCode:completion:)`。
- 删除成功后自动清除 SDK 本地 Token、用户资料并断开 MQTT。
- 保留原有 OAuth `deleteAccount(captcha:)` 接口，兼容已有客户代码。

### 1.1.0

- 增加注册、密码登录、找回密码、设置密码和修改密码接口。
- 增加 access token 并发刷新与原请求自动重试。
- 支持客户自有账号和 CLDSmart 公司账号两种接入模式。
- XCFramework 改为静态分发并提供 Swift Module Interface。
- MQTT 连接日志进行敏感信息脱敏。
