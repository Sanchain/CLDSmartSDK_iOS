//
//  APBLEDevice.h
//  FuweiCloud
//
//  Created by michael on 2024/4/29.
//

#import <Foundation/Foundation.h>
#import <VLink/APBLEDeviceType.h>

NS_ASSUME_NONNULL_BEGIN

@class CBPeripheral, APLockAllAttrResp, APBLEConfig;
@interface APBLEDevice : NSObject

@property (nonatomic, strong, readonly) CBPeripheral *peripheral;

///< 设备名称
@property (nonatomic, copy, readonly) NSString *name;

///< 设备mac地址
@property (nonatomic, copy, readonly) NSString *mac;

@property (nonatomic, copy, readonly) NSString *mac_format;

///< 设备是否绑定
@property (nonatomic, assign, readonly) BOOL binded;

@property (nonatomic, assign, readonly) APBLEDeviceMatchType match_num;

@property (nonatomic, assign, readonly) NSInteger manufaturer_id;

@property (nonatomic, strong, nullable) APLockAllAttrResp *lockAttrs;

- (instancetype)initWithAdvertisementData:(NSDictionary *)advData peripheral:(CBPeripheral *)peripheral prefixes:(NSArray<NSString *> *)prefixes;

@end

NS_ASSUME_NONNULL_END
