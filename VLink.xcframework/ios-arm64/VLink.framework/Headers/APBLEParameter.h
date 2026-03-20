//
//  APBLEParameter.h
//  FuweiCloud
//
//  Created by michael on 2024/4/22.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface APBLEParameter : NSObject

@property (nonatomic, copy, readonly) NSData *data;

+ (NSArray<NSString *> *)propertyNames;

- (instancetype)initWithData:(NSData *)data;

- (instancetype)initWithArguments:(NSArray *)arguments;

- (instancetype)initWithArguments:(NSArray *)arguments format:(NSString *)format;

- (id)objectAtIndexedSubscript:(NSUInteger)idx;

@end


#pragma pack(1)

///< 指令的通用请求数据模型
typedef struct { // int8_t
    int8_t req_code;
}APBLECommandReqInt8_t;
@interface APBLECommandReqInt8 : APBLEParameter
@property (nonatomic, assign) int8_t req_code;
@end

typedef struct { // uint8_t
    uint8_t req_code;
}APBLECommandReqUInt8_t;
@interface APBLECommandReqUInt8 : APBLEParameter
@property (nonatomic, assign) uint8_t req_code;
@end


typedef struct { // int16_t
    int16_t req_code;
}APBLECommandReqInt16_t;
@interface APBLECommandReqInt16 : APBLEParameter
@property (nonatomic, assign) int16_t req_code;
@end

typedef struct { // uint16_t
    uint16_t req_code;
}APBLECommandReqUInt16_t;
@interface APBLECommandReqUInt16 : APBLEParameter
@property (nonatomic, assign) uint16_t req_code;
@end


///< 指令的通用返回数据模型
typedef struct { // int16_t
    /*
     配网指令：0 – 网络连接成功 1 – 网络连接失败 2 – 网络连接超时 3 – 未知错 误 4 - 电量低无法配网 (配置网络(0x9001))
     设置管理员密码指令：0 - 成功 , 1 - 失败, 2-旧管理员密码错误，3-新密码格式错误(设置管理员密码(0x8018))
     获取管理员密码指令：设置管理员密码指令：0 - 未设置， 1 - 已设置(获取管理员设置状态(0x8019))
     其他指令：0 - 成功 1 - 失败
     */
    int16_t result;
}APBLECommandResp_t;
@interface APBLECommandResp : APBLEParameter
@property (nonatomic, assign) int16_t result;
@end

typedef struct { // int16_t
    /*
     其他指令：0 - 成功 1 - 失败
     */
    int16_t ret_code;
}APBLECommandResp1_t;
///< 指令的通用结果返回数据
@interface APBLECommandResp1 : APBLEParameter
@property (nonatomic, assign) int16_t ret_code;
@end



typedef struct {
    uint8_t ssid[32];
    uint8_t pw[64];
    uint8_t pk[32]; ///< 配网Token
    uint8_t timezone;
}APNetConfigReq_t;
@interface APNetConfigReq : APBLEParameter
@property (nonatomic, copy) NSString *ssid;
@property (nonatomic, copy) NSString *pw;
@property (nonatomic, copy) NSString *pk;
@property (nonatomic, assign) uint8_t timezone;
@end

typedef struct {
    uint32_t utc_time; // 时间戳，精确到秒
    int16_t time_zone; // 时区
}APSyncTimeReq_t;
@interface APSyncTimeReq : APBLEParameter
@property (nonatomic, assign) uint32_t utc_time;
@property (nonatomic, assign) int16_t time_zone;
@end


typedef struct {
    uint32_t user_id; ///用户ID
}APAddFingerprintReq_t;
@interface APAddFingerprintReq : APBLEParameter
@property (nonatomic, assign) uint32_t user_id;
@end


typedef struct {
    uint16_t pwd_id; ///< 指纹密码ID
}APDelFingerprintReq_t;
@interface APDelFingerprintReq : APBLEParameter
@property (nonatomic, assign) uint16_t pwd_id; ///< 指纹密码ID
@end

typedef struct {
    uint8_t percentage; ///< 指纹注册百分比
    uint8_t state; ///< 0 – 注册成功, 1 – 注册失败, 2- 注册超时, 3 – 注册中
    uint16_t pwd_id; ///< 指纹密码 id，如果未注册完成或注册失败则返回 0xffff，注册成功则返回 指纹密码 ID
}APFingerprintProgressResp_t;
@interface APFingerprintProgressResp : APBLEParameter
@property (nonatomic, assign) uint8_t percentage; ///< 指纹注册百分比
@property (nonatomic, assign) uint8_t state; ///< 0 – 注册成功, 1 – 注册失败, 2- 注册超时, 3 – 注册中
@property (nonatomic, assign) uint16_t pwd_id; ///< 指纹密码 id，如果未注册完成或注册失败则返回 0xffff，注册成功则返回 指纹密码 ID
@end

typedef struct {
    uint32_t user_id; ///用户ID
}APRemoteUnlockReq_t;
@interface APRemoteUnlockReq : APBLEParameter
@property (nonatomic, assign) uint32_t user_id;
@end

typedef struct {
    uint16_t error_code;///< 0 - 成功,1:失败
}APRemoteUnlockResp_t;
@interface APRemoteUnlockResp : APBLEParameter
@property (nonatomic, assign) uint16_t error_code; ///< 0 - 成功,1:失败
@end

typedef struct {
    uint8_t pwd_type;// 0 – 长期密码; 1 – 周期性密码 2 – 临时密码
    uint8_t period;//周期有效性:bit0~6 分别代表周日~周六是否开启，0 为关闭，1 为开启;
    uint16_t start_min;//周期性密码的起始时间，精确到分钟，如 9:00，则是 540;
    uint16_t end_min; //周期性密码的终止时间，精确到分钟;
    uint32_t start_timestamp;//周期性密码及临时密码有效期开始时间戳(精确到秒)
    uint32_t end_timestamp; //周期性密码及临时密码有效期结束时间戳(精确到秒)
    uint8_t passwd[32]; //数字密码信息，以’\0’为结束符
}APAddPasswordReq_t;
@interface APAddPasswordReq : APBLEParameter
@property (nonatomic, assign) uint8_t pwd_type;// 0 – 长期密码; 1 – 周期性密码 2 – 临时密码
@property (nonatomic, assign) uint8_t period;//周期有效性:bit0~6 分别代表周日~周六是否开启，0 为关闭，1 为开启;
@property (nonatomic, assign) uint16_t start_min;//周期性密码的起始时间，精确到分钟，如9:00，则是 540;
@property (nonatomic, assign) uint16_t end_min; //周期性密码的终止时间，精确到分钟;
@property (nonatomic, assign) uint32_t start_timestamp;//周期性密码及临时密码有效期开始时间戳(精确到秒)
@property (nonatomic, assign) uint32_t end_timestamp; //周期性密码及临时密码有效期结束时间戳(精确到秒)
@property (nonatomic, copy) NSString *passwd; ///< 数字密码信息
@end

typedef struct {
    uint16_t new_id;
    int16_t err_no; // 0 - 成功 , 1 - 密码不合法, 2 - 备注信息不合法, 102 - 密码信 息重复, 103 - 数据条数已达到上限, -1 - 未知错误
    uint8_t type;//0=指纹开锁、1=普通密码开锁、2=动态密码开锁、3=临时密码开锁、4=感应 卡开锁、5=电子钥匙开锁、6=虹膜开锁、7、人脸开锁、8=未知方式、9 机械钥匙，10=远程开 锁,11=周期性密码 ,12 = 管理员密码开锁，13=门内开锁，14=旋钮开锁
}APAddPasswordResp_t;
@interface APAddPasswordResp : APBLEParameter
@property (nonatomic, assign) uint16_t new_id;
@property (nonatomic, assign) int16_t err_no; // 0 - 成功 , 1 - 密码不合法, 2 - 备注信息不合法, 102 - 密码信 息重复, 103 - 数据条数已达到上限, -1 - 未知错误
@property (nonatomic, assign) uint8_t type;//0=指纹开锁、1=普通密码开锁、2=动态密码开锁、3=临时密码开锁、4=感应 卡开锁、5=电子钥匙开锁、6=虹膜开锁、7、人脸开锁、8=未知方式、9 机械钥匙，10=远程开 锁,11=周期性密码 ,12 = 管理员密码开锁，13=门内开锁，14=旋钮开锁
@end

typedef struct {
    uint16_t result;
    uint32_t user_id; //用户ID
}APNFCAddResultResp_t;
@interface APNFCAddResultResp : APBLEParameter
@property (nonatomic, assign) uint16_t result;
@property (nonatomic, assign) uint32_t user_id; //用户ID
@end

// AddFace
typedef struct {
    uint16_t ret_code;
    uint8_t step;
}APAddFaceResp_t;
@interface APAddFaceResp : APBLEParameter
@property (nonatomic, assign) uint16_t ret_code;
@property (nonatomic, assign) uint8_t step;
@end

// FaceProgressNotify
typedef struct {
    uint8_t step;
    uint8_t next_step;
    uint8_t state;
    uint16_t pwd_id;
}APFaceProgressNotifyResp_t;
@interface APFaceProgressNotifyResp : APBLEParameter
@property (nonatomic, assign) uint8_t step;
@property (nonatomic, assign) uint8_t next_step;
@property (nonatomic, assign) uint8_t state;
@property (nonatomic, assign) uint16_t pwd_id;
@end


typedef struct {
    uint16_t ID;//要删除的密码 id
    uint32_t user_id; //用户 ID;
}APDelPasswordReq_t;
@interface APDelPasswordReq : APBLEParameter
@property (nonatomic, assign) uint16_t ID;//要删除的密码 id
@property (nonatomic, assign) uint32_t user_id; //用户 ID;
@end

typedef struct {
    uint16_t ID;//要删除的密码 id
    uint32_t ret_code; //0 - 成功 , 1 - 失败
}APDelPasswordResp_t;
@interface APDelPasswordResp : APBLEParameter
@property (nonatomic, assign) uint16_t ID;//要删除的密码 id
@property (nonatomic, assign) uint32_t ret_code; //0 - 成功 , 1 - 失败
@end

typedef struct {
    uint8_t unlock_type; //0=指纹开锁、1=普通密码开锁、2=动态密码开锁、3=临时密码开锁、 4=感应 卡开锁、5=电子钥匙开锁、6=虹膜开锁、7、人脸开锁、8=未知方式、9 机械钥匙，10= 远程开锁，11=周期性密码开锁 ,12 = 管理员密码开锁，13=门内开锁，14=旋钮开锁
    uint16_t pwd_id;
}APPasswordResp_t;
@interface APPasswordResp : APBLEParameter
@property (nonatomic, assign) uint8_t unlock_type;
@property (nonatomic, assign) uint16_t pwd_id;
+(NSArray<APPasswordResp *> *)listWithData:(NSData *)data;
@end

typedef struct {
    char old_password[20]; // 旧管理员密码
    char new_password[20]; // 新管理员密码
}APAdminPwdUpdateReq_t;
@interface APAdminPwdUpdateReq : APBLEParameter
@property (nonatomic, copy) NSString *old_password;
@property (nonatomic, copy) NSString *ne_password;
@end

typedef struct {
    uint32_t local_timestamp;
    uint32_t event_id;//事件唯一 ID 值的范围从[10000,99999]，每次上报事件 ID 要在短时 间内保证不 同
    uint16_t key_id;//钥匙编号,也就每种开锁方式的唯一编号
    uint8_t unlock_type; //开锁方式
    uint8_t hijack_flag; //劫持标记。 0 未劫持， 1 已劫持， 2 无法判断。
}APUnlockEventResp_t;
@interface APUnlockEventResp : APBLEParameter
@property (nonatomic, assign) uint32_t local_timestamp;
@property (nonatomic, assign) uint32_t event_id;//事件唯一 ID 值的范围从[10000,99999]，每次上报事件 ID 要在短时 间内保证不 同
@property (nonatomic, assign) uint16_t key_id;//钥匙编号,也就每种开锁方式的唯一编号
@property (nonatomic, assign) uint8_t unlock_type; //开锁方式
@property (nonatomic, assign) uint8_t hijack_flag; //劫持标记。 0 未劫持， 1 已劫持， 2 无法判断。
@end

typedef struct {
    uint32_t local_timestamp;
    uint32_t event_id; //事件唯一 ID，范围从[10000,99999]
    uint8_t lock_type; //关锁方式: 0=门内关锁 1=旋钮关锁
}APLockEventResp_t;
@interface APLockEventResp : APBLEParameter
@property (nonatomic, assign) uint32_t local_timestamp;
@property (nonatomic, assign) uint32_t event_id; //事件唯一 ID，范围从[10000,99999]
@property (nonatomic, assign) uint8_t lock_type; //关锁方式: 0=门内关锁 1=旋钮关锁
@end

typedef struct {
    uint32_t local_timestamp;
    uint32_t event_id; //事件唯一 ID，范围从[10000,99999]
    uint8_t power_value; //电量值
}APLowPowerEventResp_t;
@interface APLowPowerEventResp : APBLEParameter
@property (nonatomic, assign) uint32_t local_timestamp;
@property (nonatomic, assign) uint32_t event_id; //事件唯一 ID，范围从[10000,99999]
@property (nonatomic, assign) uint8_t power_value;
@end

typedef struct {
    uint32_t local_timestamp;
    uint32_t event_id; //事件唯一 ID，范围从[10000,99999]
    uint32_t user_id;//用户 ID
    uint16_t key_id; //钥匙编号
}APAlterPwdEventResp_t;
@interface APAlterPwdEventResp : APBLEParameter
@property (nonatomic, assign) uint32_t local_timestamp;
@property (nonatomic, assign) uint32_t event_id; //事件唯一 ID，范围从[10000,99999]
@property (nonatomic, assign) uint32_t user_id;//用户 ID
@property (nonatomic, assign) uint16_t key_id; //钥匙编号
@end

typedef struct {
    uint32_t local_timestamp;
    uint32_t event_id; //事件唯 ID
    /* alarm_type
    * //0=指纹开锁、1=普通密码开锁、2=动态密码开锁、3=临时密码开锁、4=感应卡开锁、5=
    电子钥匙开锁、6=虹膜开锁、7、人脸开锁、8=未知方式、9 机械钥匙，10=远程开锁, 11=周期性密码开 锁,12=管理员密码开锁，13=门内开锁，14=旋钮开锁
    */
    uint8_t alarm_type;
}APUnlockExptEventResp_t;
@interface APUnlockExptEventResp : APBLEParameter
@property (nonatomic, assign) uint32_t local_timestamp;
@property (nonatomic, assign) uint32_t event_id; //事件唯 ID
@property (nonatomic, assign) uint8_t alarm_type;
@end

typedef struct {
    uint32_t local_timestamp;
    uint32_t event_id; //事件唯 ID
    uint8_t type;//0=指纹开锁、1=普通密码开锁、2=动态密码开锁、3=临时密码开锁、4=感应 卡开锁、5=电子钥匙开锁、6=虹膜开锁、7、人脸开锁、8=未知方式、9 机械钥匙，10=远程开 锁, 11=周期性密码开锁,12=管理员密码开锁，13=门内开锁，14=旋钮开锁
    uint16_t key_id; //钥匙编号
}APAddPwdEventResp_t;
@interface APAddPwdEventResp : APBLEParameter
@property (nonatomic, assign) uint32_t local_timestamp;
@property (nonatomic, assign) uint32_t event_id; //事件唯 ID
@property (nonatomic, assign) uint8_t type;
@property (nonatomic, assign) uint16_t key_id; //钥匙编号
@end

typedef struct {
    uint32_t local_timestamp;
    uint32_t event_id; //事件唯 ID
    uint16_t key_id; //钥匙编号
    uint8_t type;//0=指纹开锁、1=普通密码开锁、2=动态密码开锁、3=临时密码开锁、4=感应 卡开锁、5=电子钥匙开锁、6=虹膜开锁、7、人脸开锁、8=未知方式、9 机械钥匙，10=远程开
    //锁,11=周期性密码开锁, 12=管理员密码开锁，13=门内开锁，14=旋钮开锁
}APDelPwdEventResp_t;
@interface APDelPwdEventResp : APBLEParameter
@property (nonatomic, assign) uint32_t local_timestamp;
@property (nonatomic, assign) uint32_t event_id; //事件唯 ID
@property (nonatomic, assign) uint16_t key_id;
@property (nonatomic, assign) uint8_t type;
@end

typedef struct {
    uint32_t local_timestamp;
    uint32_t event_id; //事件唯 ID
}APCommonEventResp_t;
@interface APCommonEventResp : APBLEParameter
@property (nonatomic, assign) uint32_t local_timestamp;
@property (nonatomic, assign) uint32_t event_id; //事件唯 ID
@end

typedef struct {
    uint8_t enable; //是否设置自动回复: 0 为关闭，1 为开启
    uint8_t period;//周期有效性:bit0~6 分别代表周日~周六是否开启，0 为关闭，1 为开启;
    uint16_t start_min;//门铃周期性自动回复起始时间，精确到分钟，如 9:00，则是 540; uint16_t end_min; //门铃周期性自动回复终止时间，精确到分钟;
    uint8_t auto_res_id; //自动回复语音编号 1~5
}APAutoReplySetReq_t;
@interface APAutoReplySetReq : APBLEParameter
@property (nonatomic, assign) uint8_t enable; //是否设置自动回复: 0 为关闭，1 为开启
@property (nonatomic, assign) uint8_t period;//周期有效性:bit0~6 分别代表周日~周六是否开启，0 为关闭，1 为开启;
@property (nonatomic, assign) uint16_t start_min;
@property (nonatomic, assign) uint8_t auto_res_id;
@end

typedef struct {
    uint8_t enable; //是否设置自动回复: 0 为关闭，1 为开启
    uint8_t period;//周期有效性:bit0~6 分别代表周日~周六是否开启，0 为关闭，1 为开启;
    uint16_t start_min;//门铃周期性自动回复起始时间，精确到分钟，如 9:00，则是 540;
    uint16_t end_min; //门铃周期性自动回复终止时间，精确到分钟;
    uint8_t auto_res_id; //自动回复语音编号 1~5
}APAutoReplyGetResp_t;
@interface APAutoReplyGetResp : APBLEParameter
@property (nonatomic, assign) uint8_t enable;
@property (nonatomic, assign) uint8_t period;
@property (nonatomic, assign) uint16_t start_min;
@property (nonatomic, assign) uint16_t end_min;
@property (nonatomic, assign) uint8_t auto_res_id;
@end

typedef struct {
    uint16_t ID;
    uint16_t type;//0=指纹开锁、1=普通密码开锁、2=动态密码开锁、3=临时密码开锁、4=感应 卡开锁、5=电子钥匙开锁、6=虹膜开锁、7、人脸开锁、8=未知方式、9 机械钥匙，10=远程开锁
}APHijackCertSetReq_t;
@interface APHijackCertSetReq : APBLEParameter
@property (nonatomic, assign) uint16_t ID;
@property (nonatomic, assign) uint16_t type;
@end

typedef struct {
    int8_t battery_value; // [0 - 100] 非充电电池电量值(没有填-1)
    int8_t power_value; //[0 - 100] 充电电池电量值(没有填-1)
}APLockPowerGetResp_t;
@interface APLockPowerGetResp : APBLEParameter
@property (nonatomic, assign) int8_t battery_value;
@property (nonatomic, assign) int8_t power_value;
@end

typedef struct {
    uint8_t voice_volume; //语音导航音量 1 – 静音， 2 – 低， 3 – 中， 4 – 高
    uint8_t doorbell_volume; //门铃音量 1 – 静音， 2 – 低， 3 – 中， 4 – 高
}APLockVolume_t;

typedef struct {
    int16_t err_no;       // 错误码
    uint8_t volume;      // 音量设置
    uint8_t db_volume;   // 语音门铃音量
} APLockVolumeResponse_t;

@interface APLockVolume : APBLEParameter
@property (nonatomic, assign) uint8_t voice_volume;
@property (nonatomic, assign) uint8_t doorbell_volume;
// 新增的属性用于接收和解析响应数据
@property (nonatomic, assign) int16_t err_no;
@property (nonatomic, assign) uint8_t volume;
@property (nonatomic, assign) uint8_t db_volume;
- (instancetype)initWithResponseData:(NSData *)responseData;
@end

typedef struct {
    uint8_t front_ver[10]; // 前板版本信息
    uint8_t rear_ver[10]; // 后板版本信息
    uint8_t model[10]; // 产品型号，带结束符
}APLockVersionInfoResp_t;
@interface APLockVersionInfoResp : APBLEParameter
@property (nonatomic, copy) NSString *front_ver; // 前板版本信息
@property (nonatomic, copy) NSString *rear_ver; // 后板版本信息
@property (nonatomic, copy) NSString *model; // 产品型号，带结束符
@end

typedef struct {
    uint16_t auto_lock_time; //0 – 关闭自动闭锁， 5 ~ 900 (s) – 自动闭锁时间（默认10秒）
}APAutoLockParam_t;
typedef struct {
    int8_t result; //0 – 成功， 1 – 失败
    uint16_t auto_lock_time; //0 – 关闭自动闭锁， 5 ~ 900 (s) – 自动闭锁时间（默认10秒）
}APAutoLockResp_t;
@interface APAutoLockParam : APBLEParameter
@property (nonatomic, assign) int8_t result;
@property (nonatomic, assign) uint16_t auto_lock_time;
- (instancetype)initWithResponseData:(NSData *)responseData;
@end

typedef struct {
    uint8_t lock_status; ///0 - 开 锁 1 - 关 锁 2 - 锁 舌 收入
}APLockStateResp_t;
@interface APLockStateResp : APBLEParameter
@property (nonatomic, assign) uint8_t lock_status; ///< 0 - 成功,1:失败
@end

typedef struct {
    uint8_t lock_status; //锁闩：0 - 开锁 1 - 关锁 2 - 异常 3 - 堵转
    uint8_t door_status; //门磁：0 - 开门 1 - 关门 2 - 无门磁 3 - 异常
    uint8_t latch_status;//锁舌：0 - 收缩 1 - 伸出 2 - 无锁舌 3 - 异常 4 - 堵转
}APGetLLStatusResp_t;
@interface APGetLLStatusResp : APBLEParameter
@property (nonatomic, assign) uint8_t lock_status;
@property (nonatomic, assign) uint8_t door_status;
@property (nonatomic, assign) uint8_t latch_status;
@end


typedef struct {
    uint8_t flag; ///固定 0x00
}APOTAReq_t;
@interface APOTAReq : APBLEParameter
@property (nonatomic, assign) uint8_t flag;
@end

typedef struct {
    uint8_t flag; ///0 - 允许升级，1 - 拒绝升级
    uint16_t ota_version; ///ota协议大版本(保留，默认为 0)
    uint32_t firmware_version; ///当前锁固件版本，例如 0x00 01 00 02代表版本 V1.0.2
    uint16_t max_data_len; ///设备允许的单包最大长度，单位为字节，当前版本不超过 512 字节
}APOTAResp_t;
@interface APOTAResp : APBLEParameter
@property (nonatomic, assign) uint8_t flag;
@property (nonatomic, assign) uint16_t ota_version;
@property (nonatomic, assign) uint32_t firmware_version;
@property (nonatomic, assign) uint16_t max_data_len;
@end


typedef struct {
    uint8_t type; //固定 0x00
    uint8_t product_id[10]; //产品 ID
    uint32_t firmware_version; //升级的锁固件版本
    uint8_t file_md5[16]; //文件 MD5
    uint32_t file_data_len; //文件长度
    uint32_t file_crc32; //文件 CRC32 校验码
}APOTAFileInfoReq_t;
@interface APOTAFileInfoReq : APBLEParameter
@property (nonatomic, assign) uint8_t type;
@property (nonatomic, copy) NSString *product_id;
@property (nonatomic, assign) uint32_t firmware_version;
@property (nonatomic, copy) NSString *file_md5;
@property (nonatomic, assign) uint32_t file_data_len;
@property (nonatomic, assign) uint32_t file_crc32;
@end

typedef struct {
    uint8_t type; //固定 0x00
    uint8_t state; //0 - 正常升级， 1 - 产品 ID 不一致， 2 - 文件版本低于或等于当前版本，3- 文件大小超过范围
    uint32_t old_file_len; //已存储文件长度
    uint8_t old_crc32; //已存储文件 CRC32
    uint8_t old_md5[16]; //已存储文件 MD5
}APOTAFileInfoResp_t;
@interface APOTAFileInfoResp : APBLEParameter
@property (nonatomic, assign) uint8_t type;
@property (nonatomic, assign) uint8_t state;
@property (nonatomic, assign) uint32_t old_file_len;
@property (nonatomic, assign) uint8_t old_crc32;
@property (nonatomic, copy) NSString *old_md5;
@end

typedef struct {
    uint8_t type; //固定 0x00
    uint32_t offset; //升级文件偏
}APOTAFileOffset_t;
@interface APOTAFileOffset : APBLEParameter
@property (nonatomic, assign) uint8_t type;
@property (nonatomic, assign) uint32_t offset;
@end


typedef struct {
    uint8_t type; //固定 0x00
    uint16_t pkg_id;//前板固件总长度
    uint16_t len;//后板升级标志 0 - 没有新固件 1 - 有新固件
    uint16_t crc16;//后板固件总长度
    uint8_t data[];//后板固件总长度
}APOTADataReq_t;
@interface APOTADataReq : APBLEParameter
@property (nonatomic, assign) uint8_t type;
@property (nonatomic, assign) uint16_t pkg_id;
@property (nonatomic, assign) uint16_t len;
@property (nonatomic, assign) uint16_t crc16;
@property (nonatomic, copy) NSData *dat;
@end


typedef struct {
    uint8_t type; //固定 0x00
    uint8_t state;//0 - 成功， 1 - 包号异常， 2 - 长度不一致， 3 - CRC 校验失败 4 - 其他
}APOTADataResp_t;
@interface APOTADataResp : APBLEParameter
@property (nonatomic, assign) uint8_t type;
@property (nonatomic, assign) uint16_t state;
@end

typedef struct {
    uint8_t type; //固定 0x00
}APOTAEndReq_t;
@interface APOTAEndReq : APBLEParameter
@property (nonatomic, assign) uint8_t type;
@end


typedef struct {
    uint8_t lock_front_fw;//前板升级标志 0 - 没有新固件 1 - 有新固件
    uint32_t front_fw_data_len;//前板固件总长度
    uint8_t lock_rear_fw;//后板升级标志 0 - 没有新固件 1 - 有新固件
    uint32_t rear_fw_data_len;//后板固件总长度
}APOTAUpgradeReq_t;
@interface APOTAUpgradeReq : APBLEParameter
@property (nonatomic, assign) uint8_t lock_front_fw;
@property (nonatomic, assign) uint32_t front_fw_data_len;
@property (nonatomic, assign) uint8_t lock_rear_fw;
@property (nonatomic, assign) uint8_t rear_fw_data_len;
@end


typedef struct {
    uint8_t lock_fw_type; //1 - 前板固件 2 - 后板固件
    uint16_t pack_no;//数据包编号
    uint8_t *data; //包数据,默认 为 1 包数据为 500 个字节，最后一包以实际 长度为准
}APOTARequestReq_t;
@interface APOTARequestReq : APBLEParameter
@property (nonatomic, assign) uint8_t lock_fw_type;
@property (nonatomic, assign) uint16_t pack_no;
@property (nonatomic, copy) NSData *fw_data;
@end

typedef struct {
    uint8_t lock_fw_type; //1 - 前板固件 2 - 后板固件
    int16_t result; //升级结果:0 - 升级成功， 1 - 升级失败， 2 - 校验 失败， 3 - 空间不足 4 - 文件错误 5- 未知错误
}APOTAFinishReq_t;
@interface APOTAFinishReq : APBLEParameter
@property (nonatomic, assign) uint8_t lock_fw_type;
@property (nonatomic, assign) uint8_t result;
@end

typedef struct {
    uint8_t cmd; //指令 0x01-绑定 0x02-解绑
    uint8_t mac_addr[6]; //需要绑定/解绑的蓝牙设备 MAC 地址
    char key[8]; // 绑定使用的密码
}APDeviceBindInfoReq_t;
@interface APDeviceBindInfoReq : APBLEParameter
@property (nonatomic, assign) uint8_t cmd;
@property (nonatomic, copy) NSString *mac_addr;
@property (nonatomic, copy) NSString *key;
@end

typedef struct {
    uint8_t cmd; //指令 0x01-绑定 0x02-解绑
    uint8_t ret_code; //执行结果 0:成功 1:失败 2:参数异常
}APDeviceBindInfoResp_t;
@interface APDeviceBindInfoResp : APBLEParameter
@property (nonatomic, assign) uint8_t cmd;
@property (nonatomic, assign) uint8_t ret_code;
@end

typedef struct {
    uint16_t interval; //单位：ms，扭转电机持续时间
    uint8_t cmd; //0：开锁；1：停止
}APEmergencyUnlockReq_t;
@interface APEmergencyUnlockReq : APBLEParameter
@property (nonatomic, assign) uint16_t interval;
@property (nonatomic, assign) uint8_t cmd;
@end

typedef struct {
    uint8_t status; //状态，0：开门；1：关门；2：上锁 ；3：解锁；4：进入门锁校准 ；5 ：退出校准 ；6：进入门磁校准（固件默认与开门绑定，以防后续会有单独进行门磁校准的场景）；7：锁舌已伸出
}APAdjustLockReq_t;
@interface APAdjustLockReq : APBLEParameter
@property (nonatomic, assign) uint8_t status;
@end

typedef struct {
    int16_t result; //0-成功 1-失败
    int8_t key[8]; // 固定密码
}APBindLockResp_t;
@interface APBindLockResp : APBLEParameter
@property (nonatomic, assign) int16_t result;
@property (nonatomic, copy) NSData *key;
@end

typedef struct {
    uint8_t num_len; //third_number 长度
    uint8_t ver_len; // 版本号长度
    char *third_number; // sn 号
    char *version;// 版本号
} APVendorSNResp_t;
@interface APVendorSNResp : APBLEParameter
@property (nonatomic, assign) uint8_t num_len;
@property (nonatomic, assign) uint8_t ver_len;
@property (nonatomic, copy) NSString *third_number;
@property (nonatomic, copy) NSString *version;
@end

typedef struct {
    uint8_t result; //0 – 成功， 1 – 失败
    uint8_t latch_keep_time; //0为默认值，设备侧默认一个值，3 ~ 10 (s) –锁舌保持时间
} APLatchKeepTimeResp_t;
@interface APLatchKeepTimeResp : APBLEParameter
@property (nonatomic, assign) uint8_t result;
@property (nonatomic, assign) uint8_t latch_keep_time;
@end

typedef struct {
    int8_t key[8]; // 固定密码
}APUnbindLockReq_t;
@interface APUnbindLockReq : APBLEParameter
@property (nonatomic, copy) NSData *key;
@end

typedef struct {
    uint8_t cmd; //0 –禁用；1-启用
    uint8_t type; //0 -按键；1-指纹；3-掌纹；4-门卡
    uint8_t num; //0 –禁用；1-启用
    uint16_t keyboard_set[]; //0~255– 可对应键盘上的按键 int8_t
}APKeyBoardSetReq_t;
@interface APKeyBoardSetReq : APBLEParameter
@property (nonatomic, assign) uint8_t cmd;
@property (nonatomic, assign) uint8_t type;
@property (nonatomic, assign) uint8_t num;
@property (nonatomic, copy) NSString *keyboard_set;
@end

typedef struct {
    uint8_t backlight_set; //0 –背光灯关闭，100 亮度最大，1~99-对应背光灯亮度百分比
}APBackLightSetReq_t;
@interface APBackLightSetReq : APBLEParameter
@property (nonatomic, assign) uint8_t backlight_set;
@end

typedef struct {
    uint8_t attempt_error_reminder; //0 关闭，1~90-试错提醒次数
}APAttErrRemindSetReq_t;
@interface APAttErrRemindSetReq : APBLEParameter
@property (nonatomic, assign) uint8_t attempt_error_reminder;
@end

typedef struct {
    uint8_t status; //状态，0：开门；1：关门；2：上锁 ；3：解锁；4：进入门锁 校准 ；5 ：退出校准 ；6：进入门磁校准（固件默认与开门绑定，以防后续会有单独进行 门磁校准的场景）；7：锁舌完全收回
    uint8_t ret_code; //0-成功，1-失败
}APReportCalibrateReq_t;
@interface APReportCalibrateReq : APBLEParameter
@property (nonatomic, assign) uint8_t status;
@property (nonatomic, assign) uint8_t ret_code;
@end

typedef struct {
    uint8_t cmd; //0 –禁用；1-启用
    uint8_t type; //0 -按键；1-指纹；3-掌纹；4-门卡
    uint8_t num; //0 –禁用；1-启用
    uint16_t keyboard_set[]; //0~255– 可对应键盘上的按键 int8_t
}APGetKeyBoardSetResp_t;
@interface APGetKeyBoardSetResp : APBLEParameter
@property (nonatomic, assign) uint8_t cmd;
@property (nonatomic, assign) uint8_t type;
@property (nonatomic, assign) uint8_t num;
@property (nonatomic, copy) NSString * keyboard_set;
//@property (nonatomic, assign) NSData *keyboard_set;
@end

typedef struct {
    uint8_t backlight_set; //0 –背光灯关闭，100 亮度最大，1~99-对应背光灯亮度百分比
}APBackLightGetResp_t;
@interface APBackLightGetResp : APBLEParameter
@property (nonatomic, assign) uint8_t backlight_set;
@end

typedef struct {
    uint8_t latch_type; //是否需要校准锁舌 0：不需要（未设置，则默认值）；1：需要
    uint8_t magnetic_type; //是否需要校准门磁 0：不需要（未设置，则默认值）；1：需要
    uint8_t state; //0:未校准，1已校准
}APGetLatchTypeResp_t;
@interface APGetLatchTypeResp : APBLEParameter
@property (nonatomic, assign) uint8_t latch_type;
@property (nonatomic, assign) uint8_t magnetic_type;
@property (nonatomic, assign) uint8_t state;
@end

typedef struct {
    uint8_t attempt_error_reminder; //0 关闭，1~90-试错提醒次数
}APAttErrRemindGetResp_t;
@interface APAttErrRemindGetResp : APBLEParameter
@property (nonatomic, assign) uint8_t attempt_error_reminder;
@end

typedef struct {
    int8_t key[8]; // 固定密码
}APKeyCheckReq_t;
@interface APKeyCheckReq : APBLEParameter
@property (nonatomic, copy) NSData *key;
@end

typedef struct {
    uint32_t random_code; // 随机码
}APRandCodeResp_t;
@interface APRandCodeResp : APBLEParameter
@property (nonatomic, assign) uint32_t random_code;
@end

typedef struct {
    uint8_t passwd[32]; //数字密码信息，以’\0’为结束符
}APAdminPwdSetReq_t;
@interface APAdminPwdSetReq : APBLEParameter
@property (nonatomic, copy) NSString *passwd;
@end

/// 查询门锁产品信息(0x8071)
typedef struct {
    uint8_t serial_number[32]; // sn 号
    uint8_t secret_key[32]; // 密钥
    uint8_t product_id[16]; // 产品 ID
}APLockInfoResp_t;
@interface APLockInfoResp : APBLEParameter
@property (nonatomic, copy) NSString *serial_number;
@property (nonatomic, copy) NSString *secret_key;
@property (nonatomic, copy) NSString *product_id;
@end

typedef struct {
    uint16_t auto_lock_time; //自动闭锁时间
    uint8_t holiday_mode; //假期模式
    APLockPowerGetResp_t batt; //电量
    uint8_t doorbell_volume; //门铃音量
    APAutoReplyGetResp_t res; //门铃自动回复
    uint8_t auto_lock_enable; //自动闭锁是否开启
    uint8_t bt_mac_addr[6]; //蓝牙 mac 地址
    APLockVersionInfoResp_t info; //门锁型号和版本号
    uint16_t admin_status; //管理员设置状态 0:未设置 1:已设置
    uint8_t volume; //语音导航音量
}APLockAllAttrResp_t;
@interface APLockAllAttrResp : APBLEParameter
@property (nonatomic, assign) uint16_t auto_lock_time;
@property (nonatomic, assign) uint8_t holiday_mode;
@property (nonatomic, strong) APLockPowerGetResp *batt;
@property (nonatomic, assign) uint8_t doorbell_volume;
@property (nonatomic, strong) APAutoReplyGetResp *res;
@property (nonatomic, assign) uint8_t auto_lock_enable;
@property (nonatomic, copy) NSString *bt_mac_addr;
@property (nonatomic, strong) APLockVersionInfoResp *info;
@property (nonatomic, assign) uint8_t admin_status;
@property (nonatomic, assign) uint8_t volume;

@end

typedef  struct {
    uint32_t local_timestamp;
    uint32_t event_id;//事件唯一 ID 值的范围从[10000,99999]，每次上报事件 ID 要在短时间内保证不同
    uint16_t key_id;//钥匙编号,也就每种开锁方式的唯一编号
    uint8_t sw;//0-开锁 1-关锁
    uint8_t type; //开锁及关锁方式
}APUnLockLockEventResp_t;
@interface APUnLockLockEventResp : APBLEParameter
@property (nonatomic, assign) uint32_t local_timestamp;
@property (nonatomic, assign) uint32_t event_id;//事件唯一 ID 值的范围从[10000,99999]，每次上报事件 ID 要在短时间内保证不同
@property (nonatomic, assign) uint16_t key_id;//钥匙编号,也就每种开锁方式的唯一编号
@property (nonatomic, assign) uint8_t sw;//0-开锁 1-关锁
@property (nonatomic, assign) uint8_t type; //开锁及关锁方式

+ (NSArray<APUnLockLockEventResp *> * _Nonnull)listWithData:(NSData *)data remain:(uint16_t *)remain;

@end
typedef  struct {
    uint32_t local_timestamp;
    uint32_t event_id;//事件唯一 ID 值的范围从[10000,99999]，每次上报事件 ID 要在短时 间内保证不同
    uint8_t type; //报警类型
}APAlarmEventResp_t;
@interface APAlarmEventResp : APBLEParameter
@property (nonatomic, assign) uint32_t local_timestamp;
@property (nonatomic, assign) uint32_t event_id; //事件唯一 ID 值的范围从[10000,99999]，每次上报事件 ID 要在短时间内保证不同
@property (nonatomic, assign) uint8_t type; //报警类型

+ (NSArray<APAlarmEventResp *> * _Nonnull)listWithData:(NSData *)data remain:(uint16_t *)remain;

@end

typedef struct {
    uint8_t language; // 1-英文 2-西班牙语 (根据你的描述)
} APLanguageSetReq_t;

@interface APLanguageSetReq : APBLEParameter
@property (nonatomic, assign) uint8_t language;
@end

// --- 语言获取 Response (0x8019) ---
typedef struct {
    uint8_t language; // 0-中文 1-英文 2-西班牙语
} APLanguageGetResp_t;

@interface APLanguageGetResp : APBLEParameter
@property (nonatomic, assign) uint8_t language;
@end
typedef struct {
    uint8_t magic[2]; // "CD" = 0x43 0x44
    uint8_t version; //0x01
    uint8_t res; //保留位，默认值0x00
    uint8_t pid[32]; //pid，不足32字节用0x00补全
    uint8_t dn[32]; //device_name，，不足32个字节用0x00补全
    uint8_t paring_code[4]; // --- [新增] 设备生成的4字节配对码 ---
}APGetPidDnResp_t;
@interface APGetPidDnResp : APBLEParameter
@property (nonatomic, copy) NSString *magic;
@property (nonatomic, assign) uint8_t version;
@property (nonatomic, assign) uint8_t res;
@property (nonatomic, copy) NSString *pid;
@property (nonatomic, copy) NSString *dn;
@property (nonatomic, copy) NSString *pairingCode; // --- [新增] 供外部访问的配对码字符串 ---
@end
#pragma pack()
NS_ASSUME_NONNULL_END

