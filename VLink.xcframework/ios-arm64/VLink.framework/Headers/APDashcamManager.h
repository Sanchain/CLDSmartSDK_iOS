//
//  APDashcamManager.h
//  VLink
//
//  行车记录仪蓝牙协议管理器。
//
//  与门锁 APBLEManager 的 VLink 帧协议完全不同：本协议无同步码/长度/CRC/消息头，
//  仅向「可写特征」写入单字节指令（如 0x01），设备通过「可读特征」主动 notify 返回
//  JSON 字符串。
//
//  ⚠️ 设计要点：本管理器**持有自己独立的 CBCentralManager**，不复用 FuweiBluetooth
//  单例。原因：FuweiBluetooth 内部只有一个 central manager，靠全局 switchChannel 路由
//  发现回调，同一时刻只有一个频道在接收发现事件，无法支撑「门锁协议 + 记录仪协议」两路
//  扫描真正并行。独立 central manager 使两路扫描互不干扰、可同时进行，且 APBLEManager /
//  FuweiBluetooth 完全不动，零回归。
//

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#import <VLink/APBLEDevice.h>

NS_ASSUME_NONNULL_BEGIN

#pragma mark - 通知

///< 扫描到记录仪（object 为 APBLEDevice，source = Dashcam）
extern NSNotificationName const dashcamPeripheralDiscoverNotification;
///< 记录仪连接就绪（notify 已开启）
extern NSNotificationName const dashcamConnectedNotification;
///< 记录仪断开连接（userInfo[@"error"] 可空）
extern NSNotificationName const dashcamDisconnectNotification;
///< 收到 WiFi 信息（object 为 APDashcamWiFiInfo）
extern NSNotificationName const dashcamWiFiInfoNotification;

#pragma mark - WiFi 信息

@interface APDashcamWiFiInfo : NSObject
@property (nonatomic, copy, nullable) NSString *ssid;
@property (nonatomic, copy, nullable) NSString *pwd;
///< 原始 JSON 字符串，便于排查
@property (nonatomic, copy, nullable) NSString *rawJSON;
@end

#pragma mark - 管理器

@interface APDashcamManager : NSObject

+ (instancetype)sharedManager;

/// 按记录仪广播名规则判断名称是否命中机型白名单。
/// 广播名规则：`<[品牌 sep] 型号[变体]>_<MAC后6位>`，分隔符 '-' / '_' 混用，
/// 如 LD01_3BC1DD、LNDU-V620_067A8F、LNDU-V620Ultra_067A8F、LNDU_V820Ultra_067A8F。
/// 匹配先剥离尾部「_<6位十六进制>」拿到机型段，再按段对齐：白名单各段须作为连续子序列出现，
/// 末段允许变体后缀(V620→V620Ultra，后缀非数字开头)。既容忍品牌缩写差异(LNDU/LINGDU)与变体后缀，
/// 又靠 MAC6 后缀与段边界避免把无关设备(如 LAPTOP-..A2..)误判。
+ (BOOL)name:(nullable NSString *)name matchesDashcamModels:(nullable NSArray<NSString *> *)models;

///< 蓝牙状态
@property (nonatomic, assign, readonly) CBManagerState bleState;
///< 是否已连接
@property (nonatomic, assign, readonly) BOOL isConnected;
///< 当前连接/选中的设备（APBLEDevice，source = Dashcam）
@property (nonatomic, strong, readonly, nullable) APBLEDevice *device;

/// 开始扫描
/// - Parameter prefixes: 广播名前缀过滤（由调用方传入，如 @[@"LD01_", @"LNDU-V620_"]）
- (void)startDiscover:(NSArray<NSString *> *)prefixes;

/// 原始发现钩子：设置后，startRawDiscover 期间每发现一个外设就回调一次原始广播数据
/// （不做任何机型/前缀分类），供上层统一分类。
@property (nonatomic, copy, nullable) void(^onRawDiscover)(CBPeripheral *peripheral,
                                                          NSDictionary<NSString *, id> *advertisementData,
                                                          NSNumber *RSSI);

/// 扫描全部外设并仅通过 onRawDiscover 抛出原始广播数据（不自分类、不发 discover 通知）。
/// 用于「一次扫描 → 上层分类匹配 → 不同回调」的统一发现模型。
- (void)startRawDiscover;

/// 停止扫描
- (void)stopDiscover;

/// 连接记录仪
/// - Parameters:
///   - device: 待连接设备
///   - timeout: 超时时间（秒，<=0 不计时）
///   - completion: ready=YES 表示 notify 已开启、可收发指令
- (void)connectDevice:(APBLEDevice *)device
              timeout:(double)timeout
           completion:(nullable void(^)(BOOL ready, NSError *_Nullable error))completion;

/// 获取 WiFi 信息（发送 0x01，等待设备 notify 返回 JSON）
/// - Parameters:
///   - timeout: 超时时间（秒，<=0 不计时）
///   - completion: 成功返回 info，失败返回 error
- (void)fetchWiFiInfoWithTimeout:(double)timeout
                      completion:(nullable void(^)(APDashcamWiFiInfo *_Nullable info, NSError *_Nullable error))completion;

/// 断开连接
- (void)disconnect;

@end

NS_ASSUME_NONNULL_END
