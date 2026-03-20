//
//  LDBLEManager.h
//  APP2PDemo
//
//  Created by michael on 2023/4/17.
//

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#import <VLink/APBLEDeviceType.h>


NS_ASSUME_NONNULL_BEGIN

#define SharedBLEManager [APBLEManager sharedManager]

#define LDDataIsEmpty(data) (data == nil || [data length] < 1 ? YES : NO) // NSData 是否为空

@interface APBLEResponse : NSObject

/// 总的数据
@property (nonatomic, copy) NSData *response;

/// 同步码
@property (nonatomic, copy) NSData *syncCode;

/// 数据长度
@property (nonatomic, assign) NSInteger dataLength;

/// 接收端
@property (nonatomic, copy) NSData *destData;

/// 发送端
@property (nonatomic, copy) NSData *srcData;

/// 未加密时，一个字节，表示协议类型。加密时，四个字节，表示随机码
@property (nonatomic, copy) NSData *typeData;

/// 消息头部信息
@property (nonatomic, copy) NSData *msgData;

/// 消息头部版本
@property (nonatomic, assign) NSInteger msgVersion;

/// 消息头部类型
@property (nonatomic, assign) NSInteger msgType;

/// 消息序列号
@property (nonatomic, assign) NSInteger msgSequence;

/// 加密密钥
@property (nonatomic, copy) NSData *encryptKey;

/// 消息体部分
@property (nonatomic, copy) NSData *data;

/// 校验和
@property (nonatomic, assign) NSInteger checkSum;

/// 消息指令
@property (nonatomic, assign) APBLEVLinkCommandType command;

/// 是否超时
@property (nonatomic, assign) BOOL isTimeout;

+ (instancetype)responseWithCommand:(APBLEVLinkCommandType)command isTimeout:(BOOL)isTimeout data:(NSData *)data;

@end

typedef void(^APBLECommandResponseBlock)(APBLEResponse *response);

@class APBLEDevice;
@interface APBLEManager : NSObject

/// 连接状态回调:   -2：超时 -1：失败 0：成功
@property (nonatomic, copy, nullable) void(^connectStateBlock)(APBLEDeviceConnectStatus connectState);

/// 指令收发是否已准备（当处于加密环境时，所有指令需要在随机码指令回复后才可以发送，所以加了这个回调）
@property (nonatomic, copy, nullable) void(^commandPrepareBlock)(BOOL isCommandRepared);

/// 断连回调
@property (nonatomic, copy, nullable) void(^disconnectStateBlock)(NSError *_Nullable error);
@property (nonatomic, copy, nullable) APBLECommandResponseBlock commandResponseBlock;
@property (nonatomic, strong, readonly, nullable) APBLEDevice *device;
@property (nonatomic, assign, readonly) CBManagerState bleState;
@property (nonatomic, assign, readonly) BOOL isConnected;
@property (nonatomic, assign, readonly) BOOL isConnecting;

/// 指令收发是否已准备
@property (nonatomic, assign, readonly) BOOL isCommandRepared;
@property (nonatomic, copy, nullable) NSData *encryptKey;


/// 单例对象
+ (instancetype)sharedManager;

/// 开始扫描
/// - Parameter prefixes: 扫描前缀过滤
- (void)startDiscover:(NSArray<NSString *> *)prefixes;

/// 停止扫描
- (void)stopDiscover;

/// 连接蓝牙设备（适用于蓝牙配网场景使用）⚠️⚠️⚠️：此方法不建议使用，请使用下面的方法替代
/// - Parameters:
///   - device: 需要连接的设备
///   - config: 蓝牙设备的相关配置
///   - encryptKey: 是否需要加密密钥
///   - timeout: 超时时间
///   - connectStateBlock: 连接状态回调
- (void)connectWithDevice:(APBLEDevice *)device config:(NSDictionary *)config encryptKey:(nullable NSData *)encryptKey timeout:(double)timeout connectStateBlock:(void(^_Nullable)(APBLEDeviceConnectStatus connectState))connectStateBlock NS_DEPRECATED(2_0, 2_0, 2_0, 2_0, "请使用`connectWithDevice:(APBLEDevice *)device config:(NSDictionary *)config encryptKey:(nullable NSData *)encryptKey timeout:(double)timeout commandPrepareBlock:(void(^_Nullable)(BOOL isCommandRepared))commandPrepareBlock`");

/// 连接蓝牙设备（适用于蓝牙配网场景使用）
/// - Parameters:
///   - device: 需要连接的设备
///   - config: 蓝牙设备的相关配置
///   - encryptKey: 是否需要加密密钥
///   - timeout: 超时时间
///   - commandPrepareBlock: 连接状态回调
- (void)connectWithDevice:(APBLEDevice *)device config:(NSDictionary *)config encryptKey:(nullable NSData *)encryptKey timeout:(double)timeout commandPrepareBlock:(void(^_Nullable)(BOOL isCommandRepared))commandPrepareBlock;

/// 连接蓝牙设备（适用于进入设备详情场景使用）⚠️⚠️⚠️：此方法不建议使用，请使用下面的方法替代
/// - Parameters:
///   - bleName: 设备的名称（mac地址）
///   - config: 蓝牙设备的相关配置
///   - encryptKey: 是否需要加密密钥
///   - timeout: 超时时间
///   - connectStateBlock: 连接状态回调
- (void)connectWithName:(NSString *)bleName config:(NSDictionary *)config encryptKey:(nullable NSData *)encryptKey timeout:(double)timeout connectStateBlock:(void(^_Nullable)(APBLEDeviceConnectStatus connectState))connectStateBlock NS_DEPRECATED(2_0, 2_0, 2_0, 2_0, "请使用`connectWithName:(NSString *)bleName config:(NSDictionary *)config encryptKey:(nullable NSData *)encryptKey timeout:(double)timeout commandPrepareBlock:(void(^_Nullable)(BOOL isCommandRepared))commandPrepareBlock`");

/// 连接蓝牙设备（适用于进入设备详情场景使用）
/// - Parameters:
///   - bleName: 设备的名称（mac地址）
///   - config: 蓝牙设备的相关配置
///   - encryptKey: 是否需要加密密钥
///   - timeout: 超时时间
///   - commandPrepareBlock: 连接状态回调
- (void)connectWithName:(NSString *)bleName config:(NSDictionary *)config encryptKey:(nullable NSData *)encryptKey timeout:(double)timeout commandPrepareBlock:(void(^_Nullable)(BOOL isCommandRepared))commandPrepareBlock;

/// 断开连接
- (void)disconnect;

/// 断开连接
/// - Parameter peripheral: 指定断开的设备
- (void)disconnect:(CBPeripheral *)peripheral;

- (void)sendCommand:(APBLEVLinkCommandType)command;
- (void)sendCommand:(APBLEVLinkCommandType)command timeout:(double)timeout;

- (void)sendCommand:(APBLEVLinkCommandType)command seq:(uint16_t)seq;
- (void)sendCommand:(APBLEVLinkCommandType)command seq:(uint16_t)seq timeout:(double)timeout;

- (void)sendCommand:(APBLEVLinkCommandType)command data:(nullable NSData *)data;
- (void)sendCommand:(APBLEVLinkCommandType)command data:(nullable NSData *)data timeout:(double)timeout;

- (void)sendCommand:(APBLEVLinkCommandType)command data:(nullable NSData *)data seq:(uint16_t)seq;
- (void)sendCommand:(APBLEVLinkCommandType)command data:(nullable NSData *)data seq:(uint16_t)seq timeout:(double)timeout;



/// completion
- (void)sendCommand:(APBLEVLinkCommandType)command completion:(nullable APBLECommandResponseBlock)completion;
- (void)sendCommand:(APBLEVLinkCommandType)command timeout:(double)timeout completion:(nullable APBLECommandResponseBlock)completion;

- (void)sendCommand:(APBLEVLinkCommandType)command seq:(uint16_t)seq completion:(nullable APBLECommandResponseBlock)completion;
- (void)sendCommand:(APBLEVLinkCommandType)command seq:(uint16_t)seq timeout:(double)timeout completion:(nullable APBLECommandResponseBlock)completion;

- (void)sendCommand:(APBLEVLinkCommandType)command data:(nullable NSData *)data completion:(nullable APBLECommandResponseBlock)completion;
- (void)sendCommand:(APBLEVLinkCommandType)command data:(nullable NSData *)data timeout:(double)timeout completion:(nullable APBLECommandResponseBlock)completion;

- (void)sendCommand:(APBLEVLinkCommandType)command data:(nullable NSData *)data seq:(uint16_t)seq completion:(nullable APBLECommandResponseBlock)completion;

/// 发送指令
/// - Parameters:
///   - command: 指令枚举
///   - data: 需要携带的参数数据
///   - seq: 指令序列号
///   - timeout: 超时时间
///   - completion: 结果回调
- (void)sendCommand:(APBLEVLinkCommandType)command data:(nullable NSData *)data seq:(uint16_t)seq timeout:(double)timeout completion:(nullable APBLECommandResponseBlock)completion;


@end

NS_ASSUME_NONNULL_END
