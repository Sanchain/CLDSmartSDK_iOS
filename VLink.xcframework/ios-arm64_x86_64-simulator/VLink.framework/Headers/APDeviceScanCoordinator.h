//
//  APDeviceScanCoordinator.h
//  VLink
//
//  统一扫描协调器：一次扫描同时发现「原有设备(门锁/VLink)」与「行车记录仪」。
//
//  模型：一次扫描拿到每个外设的原始广播数据 → 对数据做分类匹配 → 统一回调 APBLEDevice，
//  调用方用 device.source 判断设备来源（门锁 / 记录仪）。
//    · 由 APDashcamManager 的单个 CBCentralManager 扫全部（startRawDiscover），逐个抛原始广播；
//    · 本协调器对每条广播分类：
//        记录仪 = 名称 containsString 机型白名单(调用方传参) → APBLEDevice(source=Dashcam)
//        门锁   = 名称 hasPrefix 前缀(调用方传参) 且 有厂商数据 → APBLEDevice(source=Lock，沿用其解析)
//  单次扫描、单个 central：避免两路并扫冲突，记录仪发现与连接同源、可直接连。
//

#import <Foundation/Foundation.h>
#import <VLink/APBLEDevice.h>

NS_ASSUME_NONNULL_BEGIN

/// 统一发现通知，object 为 APBLEDevice（用 source 判断来源）
extern NSNotificationName const apDeviceDiscoverNotification;

@interface APDeviceScanCoordinator : NSObject

+ (instancetype)sharedCoordinator;

/// 每发现一个设备回调一次（主线程）。用 device.source 判断是门锁还是记录仪。
@property (nonatomic, copy, nullable) void(^onDiscover)(APBLEDevice *device);

/// 一次扫描发现门锁 + 记录仪，按规则分类后统一回调。
/// - Parameters:
///   - lockPrefixes: 门锁名称前缀（hasPrefix + 有厂商数据，沿用 APBLEDevice 解析）；为空/nil 不识别门锁。
///   - dashcamModels: 记录仪机型白名单（containsString，与源码一致）；为空/nil 不识别记录仪。
- (void)startDiscoverLockPrefixes:(nullable NSArray<NSString *> *)lockPrefixes
                    dashcamModels:(nullable NSArray<NSString *> *)dashcamModels;

/// 停止扫描。
- (void)stopDiscover;

@end

NS_ASSUME_NONNULL_END
