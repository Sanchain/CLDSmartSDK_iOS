//
//  APBLEDevice.h
//  FuweiCloud
//
//  Created by michael on 2024/4/29.
//

#import <Foundation/Foundation.h>
#import <VLink/APBLEDeviceType.h>

NS_ASSUME_NONNULL_BEGIN

/// 设备来源（用于判断这条发现记录是门锁还是行车记录仪）
typedef NS_ENUM(NSInteger, APBLEDeviceSource) {
    APBLEDeviceSourceLock = 0,    ///< 原有设备（门锁 / VLink）
    APBLEDeviceSourceDashcam = 1, ///< 行车记录仪
};

@class CBPeripheral, APLockAllAttrResp, APBLEConfig;
@interface APBLEDevice : NSObject

///< 设备来源：默认 Lock；行车记录仪用 initWithDashcamAdvertisementData: 构造时为 Dashcam
@property (nonatomic, assign, readonly) APBLEDeviceSource source;

@property (nonatomic, strong, readonly) CBPeripheral *peripheral;

///< 设备名称
@property (nonatomic, copy, readonly) NSString *name;

///< 设备mac地址
@property (nonatomic, copy, readonly) NSString *mac;
///< 带前缀的mac地址
@property (nonatomic, copy, readonly) NSString *mac_prefix;
@property (nonatomic, copy, readonly) NSString *mac_format;

///< 设备是否绑定
@property (nonatomic, assign, readonly) BOOL binded;

@property (nonatomic, assign, readonly) APBLEDeviceMatchType match_num;

@property (nonatomic, assign, readonly) NSInteger manufaturer_id;

@property (nonatomic, strong, nullable) APLockAllAttrResp *lockAttrs;

- (instancetype)initWithAdvertisementData:(NSDictionary *)advData peripheral:(CBPeripheral *)peripheral prefixes:(NSArray<NSString *> *)prefixes;

/// 行车记录仪来源构造（source = Dashcam）。
/// 取 SSID 仅需 3 个设备级数据：peripheral(连接,必需)、name(展示/机型匹配)、mac(唯一标识/去重)。
/// 服务/特征 UUID 与指令码 0x01 为固定常量(见 APDashcamManager)，不记录；门锁专用字段保持默认。
- (instancetype)initWithDashcamAdvertisementData:(NSDictionary *)advData peripheral:(CBPeripheral *)peripheral;

@end

NS_ASSUME_NONNULL_END
