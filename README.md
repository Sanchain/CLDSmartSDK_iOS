# CLDSmartSDK_iOS

CLDSmartSDK iOS SDK 提供账号认证、设备绑定、蓝牙通信、IoT 控制、消息推送和音视频能力。

- 当前版本：`1.4.17`
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
      :tag => '1.4.17'
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
    deviceToken: apnsToken,  // 尚未取得 APNs Token 时传空字符串
    isAPNsSandbox: true      // 按 aps-environment 设置，与业务服务器环境无关
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

## 共享成员与设备转移

`updateShareMemberList` 提交的是设备当前应保留的完整非空共享成员标识列表，不是要新增或删除的差量。为兼容旧客户，从 `1.4.5` 开始传入空数组会在 SDK 本地返回 `false`，不会请求服务端。

删除一个共享成员请使用显式接口：

```swift
engine.removeShareMember(vid: deviceVID, memberId: targetMemberID) { success in
    guard success else {
        print("删除共享成员失败")
        return
    }
}
```

明确清空全部共享成员时，客户 App 必须先做二次确认，再调用：

```swift
engine.clearShareMemberList(vid: deviceVID) { success in
    guard success else {
        print("清空共享成员失败")
        return
    }
}
```

三个方法复用现有的共享成员全量覆盖服务端接口，不要求后端增加路由。`removeShareMember` 会先读取最新列表，再提交删除后的完整列表，包括删除最后一人时的空列表。从 `1.4.6` 开始，SDK 会在当前 App 进程内按 VID 串行执行添加、删除和清空；不同 VID 仍可并行。客户界面仍应避免重复提交。多台手机或多个 App 进程同时修改同一 VID 时，本地队列无法避免全量列表互相覆盖，彻底解决需要服务端提供版本校验或原子增删接口。

设备所有权转移先调用 `fetchChangeOwnerCode`，并且 `account` 与 `identify` 至少传一个。成功取得 `event_code` 只代表账户确认完成；最终转移仍需完成验证码流程并调用 `verifyChangeOwner`。

```swift
engine.fetchChangeOwnerCode(
    vid: deviceVID,
    account: targetAccount
) { eventCode in
    guard let eventCode else {
        print("获取设备转移授权码失败")
        return
    }

    // 获取并展示验证码后，将用户输入的验证码传给 verifyChangeOwner。
    engine.verifyChangeOwner(
        code: eventCode,
        captcha: captcha
    ) { success in
        print("设备转移结果：\(success)")
    }
}
```

`1.4.4` 修复了账户确认接口缺少 `devices/` 路径段导致的 HTTP 404。

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

## BLE 连接与远程开锁并发

从 `1.4.6` 开始，`connectBLE(name:config:encryptKey:timeout:completion:)` 和 `connectBLE(device:config:encryptKey:timeout:completion:)` 共用同一个全局连接协调器，completion 固定在主线程且最多回调一次。VLink 使用全局 BLE 管理器，因此两个入口合计同一时刻只接受一笔连接任务；重叠调用立即返回 `false`，不会取消当前任务。连接完成会取消对应的 SDK 兜底超时任务；超时后请等待 completion 返回再重试，SDK 会先清理旧连接，避免旧断开事件影响新连接。`device` 入口现在会使用调用方传入的 `timeout`，且允许 completion 为 `nil`。

`unlockDevice(vid:unLock:timeout:completion:)` 的 completion 同样固定在主线程且最多回调一次。不同 VID 的监听、计时器和任务状态相互隔离，可以并行；同一 VID 已有任务时，新调用立即返回 `false`，不会中断当前任务。状态等待超时从服务端确认受理后开始计算，与旧版语义一致。

## 指纹录入与列表同步

指纹业务按“BLE 就绪 -> 录入成功 -> 本地列表上报 -> 云端列表查询”执行。`bleIsConnected` 只代表物理连接；发送业务命令前必须确认 `bleIsReady == true`。`progress == 100` 时状态仍可能是 `.inputting`，只有 `.inputSuccess` 才是最终成功。

```swift
func enrollFingerprint(device: CLDDevice, name: String) {
    let engine = CldSmartEngine.shared
    let fingerprint = CLDPassword(
        unlock_type: CLDInputKeyType.fingerprint.rawValue,
        note: name
    )

    guard engine.bleIsReady else {
        print("BLE 尚未完成 VLink 握手")
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
                print(message ?? "同步设备指纹失败")
                return
            }

            engine.getDeviceKeyList(
                vid: device.vid,
                type: .fingerprint
            ) { list, code, message in
                guard code == 20000, let list else {
                    print(message ?? "查询云端指纹列表失败")
                    return
                }
                print("设备本地指纹数：\(localCount)，云端指纹数：\(list.count)")
            }
        }
    }
}
```

首次进入页面或尚未连接时，先使用设备的 `bluetooth_id`、`input_bt_config ?? bt_config` 和 `bt_secret` 调用 `connectBLE`；其成功回调后仍应检查 `bleIsReady`。不要在已有连接或连接中的状态下重复调用 `connectBLE`。`getDeviceKeyList` 只查询云端数据，不直接读取设备；BLE 录入后应先调用 `syncDeviceKeyListFromBLE`。不要把 `bt_secret`、Token 或原始 BLE 数据写入日志。

## 纯蓝牙设备 OTA

DL500（`match_num == 11`）、Keypad（`13`）、KeypadP（`74`）以及其他主 `bt_config.protocol == 1` 且固件支持 C010-C014 的设备，统一使用 `upgradeBLEDevice`。WiFi/联网设备继续使用 MQTT OTA `upgradeDevice`；不要用 `match_num` 判断 OTA 传输方式。

```swift
let engine = CldSmartEngine.shared

guard device.bt_config.protocol == 1 else {
    // WiFi/联网设备调用 upgradeDevice。
    return
}

engine.upgradeBLEDevice(vid: device.vid, timeout: 5) {
    progress, status, errorCode, message in
    switch status {
    case .success:
        // 只有设备 C014 整包校验返回 0 才会进入 success。
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

SDK 会根据 `vid` 查询设备和固件信息、校验主 `bt_config.protocol == 1`、下载固件、必要时自动连接目标蓝牙，并执行 `C010` 至 `C014`。进度范围为 `0...100`：下载占 `0...49`，BLE 传输占 `50...99`，C014 校验成功后为 `100`。

- `.success` 是唯一成功终态，表示 C014 未超时且 `response.data[1] == 0`。
- `.verifying` 仍在等待设备整包校验，不能提示成功。
- 云端 OTA 状态上报只用于后台记录，不是设备成功依据。
- 同时只能进行一笔 BLE OTA；可用 `bleOTAIsRunning` 防止重复点击。
- 用户取消时调用 `cancelBLEOTA()`，原升级回调会收到 `.cancelled`。
- 升级期间保持 App 在前台、设备唤醒且电量充足，不要并发发送其他 BLE 指令。

从 `1.4.11` 起，客户现场可按 `[CLDSmartSDK][BLEOTA]` 过滤日志。日志包含同一次升级的操作编号、累计耗时、固件字节数、协商分包长度、当前包号/总包数、重试次数、最近 RTT、最近响应和 BLE 连接状态，不包含固件内容、蓝牙密钥或账号凭证。默认 `timeout=5` 时，同一 C013 分包约 15 秒无进展会返回 `.failed` 与错误码 `-5112`，不会无限停在传输中。

Keypad/KeypadP 扫描结果的 `binded` 直接来自 manufacturer data 第 4 字节，不读取 SDK 缓存或云端列表。例如 `FFFF010100000D` 中的第 4 字节 `01` 表示设备仍广播“已绑定”，真正恢复未绑定后应为 `FFFF010000000D`。如果设备端重置后仍广播 `01`，应检查是否执行完整恢复出厂、是否重启并刷新广播，以及设备固件是否清除了本地绑定标志。

`1.4.9` 的 `upgradeDL500Device`、`cancelDL500OTA`、`dl500OTAIsRunning` 及旧 DL500 类型仍可编译，但已标记为 deprecated 并转发到通用 BLE OTA。WiFi/联网设备继续使用现有 `upgradeDevice(vid:timeout:completion:)`，行为不变。

## APNs 推送 Token

App 应启用 Push Notifications Capability，并注册远程通知。登录请求中的 `device_token`
和 `reg_token` 使用同一个 APNs Token。Token 可能早于或晚于登录返回，因此客户 App
应先缓存 Token，只在 `initEngine` 和密码登录均成功后同步。

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

    // 如果 Token 在登录后取得，立即通知服务端更新；登录前只缓存。
    if CldSmartEngine.shared.isLoggedIn {
        CldSmartEngine.shared.refreshAPNs(token: token) { success in
            print("APNs token updated: \(success)")
        }
    }
}
```

密码登录成功后，如果 Token 已经缓存，应再调用一次 `refreshAPNs(token:completion:)`。
`isAPNsSandbox` 必须按已安装 App 签名中的 `aps-environment` 设置：值为
`development` 时传 `true`，值为 `production` 时传 `false`。它与 `.us`、
`isDevServer` 等业务服务器配置相互独立。SDK 在 Token 为空时会生成唯一占位值，
但占位值不能接收推送。

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

### 1.4.17

- 修复 Release 版 SDK 在刷新 APNs Token 时固定选择 Production 的问题。账号密码登录成功后会保存 `CLDAuthContext.isAPNsSandbox`，后续刷新自动复用同一环境。
- `isAPNsSandbox` 现在明确按 App 签名的 `aps-environment` 设置，与业务测试/正式服务器无关；`ios_dev=1` 表示 Sandbox，`ios_dev=0` 表示 Production。
- 登录和刷新都会清理 Token 中的尖括号与空白字符，诊断日志只输出环境、`ios_dev` 和 Token 长度，不输出 Token 原文。
- 公开 API 签名不变；旧 `login(account:)` 和升级前缓存的登录态保留旧版默认行为。使用账号密码登录的客户升级后应重新登录一次，再同步 APNs Token。
- Device arm64、Simulator arm64/x86_64 XCFramework、Swift Interface、CocoaPods lint 和临时客户工程 Release 编译均已验证；最终推送仍需真机确认后台选择正确 APNs 环境并收到 Apple HTTP 200。

### 1.4.16

- 新增 Google Assistant 与 Alexa 云端授权 API：`getVoiceAssistantAvailability`、`getVoiceAssistantLinkStatus`、`getAlexaAuthorizationLinks`、`completeAlexaLink`、`authorizeAlexaSkill`、`getGoogleAssistantAuthorizationInfo`、`unlinkVoiceAssistant` 和 `setGoogleAssistantPIN`。
- 新增 `CLDVoiceAssistant`、`CLDVoiceAssistantAvailability`、`CLDVoiceAssistantLinkStatus`、`CLDAlexaAuthorizationLinks`、`CLDAlexaSkillAuthorizationRequest`、`CLDAlexaSkillAuthorizationResult` 和 `CLDGoogleAssistantAuthorizationInfo` 公开模型。
- SDK 日志会隐藏 OAuth 授权码、`state`、授权 URL、Deep Link 和语音 PIN；客户 App 负责打开授权入口、接收 Universal Link，并使用客户专属 Client ID、Alexa Skill、Google Action 和真实账号完成授权闭环。
- Voice Assistants Demo 页面、9 个输入字段和 10 个 API 操作入口已通过 UI 自动化；Device arm64、Simulator arm64/x86_64 XCFramework、Swift Interface、CocoaPods lint 和临时客户工程编译均已验证。
- 无账号的 Simulator 自动化不覆盖真实 Alexa/Google OAuth 生命周期；正式接入前仍需验证授权、回调、绑定状态、解绑和 Google PIN。

### 1.4.15

- 新增 `CLDSessionInvalidReason`、`sessionInvalidHandler`、`lastSessionInvalidReason` 和 `Notification.Name.cldSessionDidInvalidate`，客户 App 可统一监听必须重新登录的会话终态。
- `40101` 先由 SDK 自动刷新 Access Token；只有刷新失败或重试仍失败才清除会话并通知客户。`40102` 表示账号在其他客户端登录，`40105` 表示 Refresh Token 已过期，两者会立即清除会话并通知客户。
- 会话失效回调固定在主线程，回调前 SDK 已清除本地用户信息并断开 MQTT；清除后调用其他公开 API 仍返回最近的终态错误码。
- 修复并发请求重复发送失效事件，以及旧账号迟到响应误清除新账号会话的问题。
- 继续提供 `markNotificationAsRead(notifyId:completion:)` 和 `deleteNotifications(notifyIds:completion:)` 通知管理接口。
- Device arm64、Simulator arm64/x86_64 XCFramework、Swift Interface、SDK/Demo 编译和 CocoaPods 临时客户工程均已验证。

### 1.4.14

- 新增 `markNotificationAsRead(notifyId:completion:)`，使用 `CLDEvent.notify_id` 将单条通知标记为已读。
- 新增 `deleteNotifications(notifyIds:completion:)`，支持一次删除 1 到 100 条通知；删除不可恢复，客户 App 应在调用前二次确认。
- 两个接口统一返回 `success/code/message`；非法 ID 在本地返回 `code == -1000`，不会发送网络请求。
- Demo 的 Events 页面和客户 API 目录已增加可运行示例、参数校验与删除确认。
- Device arm64、Simulator arm64/x86_64 XCFramework、Swift Interface 和 CocoaPods 临时客户工程均已验证。

### 1.4.13

- `CLDLockCalibrationResult` 新增 `requiresLatchCalibration`、`isMagneticCalibrated` 和 `isCalibrated`，客户页面无需解释 `9077` 原始字节。
- 明确 `magneticType == 1` 表示门磁已校准；修正并弃用含义容易混淆的 `magneticNeedsCalibration`。
- 校准会话现在按 `CLDLockCalibrationProfile.identifier` 选择内部协议适配器；未知 profile 返回 `unsupported`，不会误用标准锁协议。
- Demo 的紧急解锁收尾改为调用 `fetchLockCalibrationStatus`，不再自行发送和解析 `9077`。
- Device arm64、Simulator arm64/x86_64 XCFramework、Swift Interface、SDK 与 Demo 编译和 CocoaPods 临时客户工程均已验证。

### 1.4.12

- 新增不绑定具体锁型号的 `CLDLockCalibrationSession`，覆盖手动/自动校准、锁舌/开门方式、门磁、解锁测试、上锁测试、取消和完成。
- 新增 `CLDLockCalibrationStage`、`CLDLockCalibrationEvent`、`CLDLockCalibrationError` 和 `CLDLockCalibrationResult`；客户页面只处理 typed 业务结果，不解析蓝牙 `Data`。
- `complete` 会退出校准并自动读取最终 `latchType`、`magneticType`、`state`；紧急解锁或其他门锁流程完成后可通过 `fetchFinalStatus` 读取相同结果。
- 新增 `CLDLockCalibrationProfile` 作为未来锁型号协议适配入口，公共 API 不使用型号专用命名。
- 设备 arm64、模拟器 arm64/x86_64 XCFramework、Swift Interface、Demo 客户页面编译验证已通过。

### 1.4.11

- 修复 `1.4.10` 中连续 C013 分包可能丢失下一包回调和定时器、导致 OTA 永久停在 `.transferring` 的问题；客户调用方式不变。
- 新增 `[CLDSmartSDK][BLEOTA]` 结构化诊断日志，覆盖固件下载、BLE 连接、C010-C014、分包进度、重试、RTT、响应、BLE 状态及最终错误。
- 新增独立 C013 无进展 watchdog；默认 `timeout=5` 时约 15 秒无进展返回 `.failed/-5112`。
- OTA 实现文件更名为 `CldEngineBluetoothOTAEx.swift`，体现它适用于所有满足传输配置和协议能力的纯蓝牙设备；扫描、连接和通用命令仍由 `CldEngineBluetoothEx.swift` 负责。
- 补充 Keypad/KeypadP manufacturer data 绑定位诊断。`FFFF010100000D` 表示设备广播已绑定，`FFFF010000000D` 才表示未绑定。
- Device arm64、Simulator arm64/x86_64 XCFramework、Swift Interface、CocoaPods lint 和客户工程编译已通过。真实 DL500、Keypad 与 KeypadP 固件仍需完成真机 OTA 验收。

### 1.4.10

- 将 DL500 专用 OTA 泛化为纯蓝牙设备 OTA：DL500（`match_num == 11`）、Keypad（`13`）、KeypadP（`74`）及其他主 `bt_config.protocol == 1` 且支持 C010-C014 的设备统一调用 `upgradeBLEDevice(vid:timeout:completion:)`。
- 新增通用 `CLDBLEOTAStatus`、`CLDBLEOTAErrorCode`、`bleOTAIsRunning` 和 `cancelBLEOTA()`；旧 DL500 API 与类型保留并标记为 deprecated，现有客户代码可继续编译。
- OTA 传输选择不再写死型号：主 `bt_config.protocol == 1` 走 BLE C010-C014；WiFi/联网设备继续调用 `upgradeDevice` 走 MQTT。
- 设备升级成功严格要求 C014 未超时、回包至少 2 字节且 `response.data[1] == 0`；下载完成、C013 传输完成、进度到 99 或云端上报成功都不算设备升级成功。
- Device arm64、Simulator arm64/x86_64 XCFramework 校验和 CocoaPods 临时客户工程编译通过。真实 DL500、Keypad 与 KeypadP 固件仍应在发布接入前完成真机验收。

### 1.4.9

- 新增 `upgradeDL500Device(vid:timeout:completion:)`，封装 DL500 固件查询、HTTP 下载、目标 BLE 自动连接以及 C010-C014 完整 OTA 流程。
- 新增 `CLDDL500OTAStatus`、`CLDDL500OTAErrorCode`、`dl500OTAIsRunning` 和 `cancelDL500OTA()`；进度和终态回调固定在主线程。
- DL500 成功只认 C014 整包校验返回 0；下载完成、C013 传输完成和云端状态上报成功均不作为设备升级成功。
- 增加单任务互斥、回包长度校验、首尾包边界处理、每包独立超时重试、BLE 断连/后台/取消收尾以及错误码。
- 现有 WiFi/联网设备 `upgradeDevice` MQTT OTA 方法和行为不变。

### 1.4.8

- 配件绑定与解绑按真实传输方式分流：`bindWiFiAccessory`、`unbindWiFiAccessory` 保持原 WiFi 服务端参数，不发送 `is_blue`；BLE 配件方法由 SDK 内部发送 `is_blue=1`。
- Keypad/DL500 套装使用 BLE 配件关系：Keypad 为主设备，DL500 为配件。其他 WiFi 配件继续调用 WiFi 方法，现有 MQTT 确认流程不变。
- 客户 App 无需也不应自行拼接 `is_blue`；现有公开方法签名未修改，未使用配件功能的客户不受影响。
- device arm64、simulator arm64/x86_64 XCFramework 结构校验以及 CocoaPods 临时工程编译通过。

### 1.4.7

- `CLDDeviceInfo` 新增可选字段 `is_online` 和 `power`，用于读取设备详情接口返回的云端在线状态和电量快照；旧响应缺少字段时为 `nil`，`power_status` 语义不变。
- 新增 `formatSDCard(vid:completion:)`。HTTP 成功仅表示请求已受理，最终以 MQTT `stored_reset` 事件且 `params.result == 1` 为成功。
- 修复 `MediaCore` 录像器生命周期，停止、失败和销毁后完整释放，支持再次录像和退出直播后重建；公开方法签名不变。
- OTA 请求成功并订阅 MQTT 后立即启动已有超时计时器，修复首个计时周期未开始的问题。
- 修复模拟器 `CLDSmartSDK-Swift.h` 的完整性，设备 arm64、模拟器 arm64/x86_64 均通过 Clang 检查。
- 本版本未删除接口或修改已有方法签名，原有客户更新完整 XCFramework/Pod 后重新编译即可。若直接穷举 `NetworkAPI`，需补充 `.resetSDCard` 或 `@unknown default`。

### 1.4.6

- 共享成员的更新、单成员删除和全部清空在当前 App 进程内按 VID 串行，避免同一 VID 的并发全量写请求互相覆盖；不同 VID 仍可并行。
- 两个 `connectBLE` 重载共用全局连接协调器，重叠调用返回 `false`；每笔任务使用独立 ID，完成时取消对应超时任务，completion 固定主线程且最多一次。
- `connectBLE(device:...)` 改为使用调用方传入的 `timeout`，并安全支持 `nil` completion。
- 公开 Swift Interface 和 Objective-C 头文件与 `1.4.5` 一致，客户无需修改方法签名。

### 1.4.5

- 恢复 `updateShareMemberList` 的旧版空数组保护；空数组本地返回 `false`，不再隐式清空共享成员。
- 新增 `removeShareMember` 和 `clearShareMemberList`，分别显式删除单个成员和清空全部成员，继续复用现有服务端全量覆盖接口。
- BLE 名称连接增加独立任务 ID、可取消超时任务和断开清理窗口，completion 固定主线程且最多一次，重叠连接不会中断当前任务。
- 远程开关锁按 VID 隔离监听、计时器和任务状态；不同 VID 可并行，同一 VID 的重叠调用返回 `false`，旧任务不能结束新任务。

### 1.4.4

- 修复设备所有权转移账户确认接口路径，避免 `fetchChangeOwnerCode` 返回 HTTP 404。
- 修复删除最后一个共享成员时 SDK 直接返回 `false`；`updateShareMemberList` 现在允许用空数组清除全部共享成员。
- 补充共享成员全量覆盖语义和设备转移调用说明。

### 1.4.3

- 修复指纹列表请求类型，`.fingerprint` 现在按服务端约定发送 `finger_print`。
- 新增 `bleIsReady`，用于区分物理连接和已完成 VLink 随机码/密钥握手的可用连接。
- 新增 `syncDeviceKeyListFromBLE`，读取门锁本地凭证并同步至云端后再查询列表。
- 修复 BLE 指纹录入进度与 busy 状态处理；`progress=100` 不再被误判为最终成功。
- SDK 与 VLink 使用静态链接，避免宿主 App 同时加载重复 Objective-C 类。

### 1.4.2

- `CLDPassword` 增加可选可写字段 `is_duress_password`，用于添加胁迫密码。
- 创建密码后设置 `password.is_duress_password = true`，`addDigitsPassword` 会自动向服务端发送该字段；未设置时保持原有请求不变。

### 1.4.1

- `CLDDeviceKey` 增加可选字段 `is_duress_password`，用于识别胁迫密码、胁迫指纹和胁迫掌静脉等凭证。
- 兼容服务端返回的 `true/false`、`0/1` 和旧字段 `is_duress`；字段缺失时返回 `nil`。
- 访客密码仍通过 `unlock_type == .tmpPassword` 判断，普通用户密码通过 `unlock_type == .password` 判断。

### 1.4.0

- 修复临时密码、周期密码等凭证导致 `getDeviceKeyList` 返回空数组的问题。
- `CLDDeviceKey.unlock_type` 现在返回具体的 `CLDDeviceUnlockType`，可区分普通密码、TOTP 密码、临时密码、周期密码和管理员密码。
- 增加 `CLDDeviceKey.unlock_type_raw`，服务端新增未知类型时仍保留原始整数，避免整页凭证解析失败。
- 密码列表查询统一发送 `key_type=password`，并改为逐条解析，单条异常数据不再清空整个列表。
- 增加数字密码受理结果和 MQTT/网络诊断信息，便于区分服务端受理、设备执行及列表同步状态。

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
