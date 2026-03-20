//
//  APBLEDeviceType.h
//  FuweiCloud
//
//  Created by michael on 2024/8/12.
//

//蓝牙设备状态改变通知
static NSNotificationName bleCentralStateDidUpdateNotification = @"bleCentralStateDidUpdateNotification";

//外设搜索回调通知
static NSNotificationName blePeripheralDiscoverNotification = @"blePeripheralDiscoverNotification";

//外设连接通知
static NSNotificationName blePeripheralConnectedNotification = @"blePeripheralConnectedNotification";

//外设断开连接通知
static NSNotificationName blePeripheralDisconnectNotification = @"blePeripheralDisconnectNotification";

//收到指令响应通知
static NSNotificationName bleReceivedCommandDataNotification = @"bleReceivedCommandDataNotification";

//蓝牙监听成功通知
static NSNotificationName  blePeripheralDidUpdateNotification = @"blePeripheralDidUpdateNotification";


///< 蓝牙设备配网参数
typedef NS_ENUM(NSInteger, APBLEDeviceMatchType) {
    APBLEDeviceMatchTypeAL501 = 1,
    APBLEDeviceMatchTypeAL601 = 2,
    APBLEDeviceMatchTypeD289 = 3,
    APBLEDeviceMatchTypeK27 = 4,
    APBLEDeviceMatchTypeK28 = 5,
    APBLEDeviceMatchTypeAL701 = 6,
    APBLEDeviceMatchTypeHK01 = 7,
    APBLEDeviceMatchTypeC710 = 8,
    APBLEDeviceMatchTypeC630 = 9,
    APBLEDeviceMatchTypeD100 = 10,
    APBLEDeviceMatchTypeDL500 = 11,
    APBLEDeviceMatchTypeK26 = 12,
    APBLEDeviceMatchTypeKeypad = 13,
    APBLEDeviceMatchTypeSP300 = 14,
    APBLEDeviceMatchTypeST380 = 15,
    APBLEDeviceMatchTypeK01 = 16,
    APBLEDeviceMatchTypeP160M = 17,
    APBLEDeviceMatchTypeK15 = 18,
    APBLEDeviceMatchTypeK16 = 19,
    APBLEDeviceMatchTypeK25 = 20,
    APBLEDeviceMatchTypeSF301 = 21,
    APBLEDeviceMatchTypeD288W = 22,
    APBLEDeviceMatchTypeC600 = 23,
    APBLEDeviceMatchTypeK27N = 24,
    APBLEDeviceMatchTypeS398 = 25,
    APBLEDeviceMatchTypeSF300L = 26,
    APBLEDeviceMatchTypeH05C = 27,
    APBLEDeviceMatchTypeD180 = 28,
    APBLEDeviceMatchTypeD280 = 29,
    APBLEDeviceMatchTypeDSM402 = 30,
    APBLEDeviceMatchTypeS390 = 31,
    APBLEDeviceMatchTypeSF03C = 32,
    APBLEDeviceMatchTypeSF05C = 33,
    APBLEDeviceMatchTypeL600B = 34,
    APBLEDeviceMatchTypeDL300 = 35,
    APBLEDeviceMatchTypeL800 = 36,
    APBLEDeviceMatchTypeK07 = 37,
    APBLEDeviceMatchTypeC700 = 38,
    APBLEDeviceMatchTypeHK01W = 39,
    APBLEDeviceMatchTypeSF302 = 40,
    APBLEDeviceMatchTypeAL501C = 41,
    APBLEDeviceMatchTypeAL601C = 42,
    APBLEDeviceMatchTypeAL505C = 43,
    APBLEDeviceMatchTypeAL605C = 44,
    APBLEDeviceMatchTypeBS03 = 45,
    APBLEDeviceMatchTypeDF511 = 46,
    APBLEDeviceMatchTypeDF515 = 47,
    APBLEDeviceMatchTypeDF360 = 48,
    APBLEDeviceMatchTypeDL301 = 49,
    APBLEDeviceMatchTypeDL500J = 50,
    APBLEDeviceMatchTypeK11 = 51,
    APBLEDeviceMatchTypeL600 = 52,
    APBLEDeviceMatchTypeK27B = 53,
    APBLEDeviceMatchTypeS500 = 54,
    APBLEDeviceMatchTypeSP200 = 55,
    APBLEDeviceMatchTypeSP201 = 56,
    APBLEDeviceMatchTypeAH1 = 57,
    APBLEDeviceMatchTypeHK01P = 58,
    APBLEDeviceMatchTypeC630C = 59,
    APBLEDeviceMatchTypeH10Max = 60,
    APBLEDeviceMatchTypeDL500JW = 61,
    APBLEDeviceMatchTypeB312US = 68,
    APBLEDeviceMatchTypeB731JPK = 69,
    APBLEDeviceMatchTypeB731JP = 70,
    APBLEDeviceMatchTypeB721JP = 71,
    APBLEDeviceMatchTypeB721JPK = 72,
    APBLEDeviceMatchTypeK26SL = 73,
    APBLEDeviceMatchTypeKeypadP = 74,
    APBLEDeviceMatchTypeS398DLC = 75,
    APBLEDeviceMatchTypeC630D = 76,
    APBLEDeviceMatchTypeDoorBellDuo = 81,
    
    APBLEDeviceMatchTypeD288 = 999999,
};


typedef NS_ENUM(int8_t, APBLEDeviceConnectStatus) {
    APBLEDeviceConnectStatus_Success = 0,
    APBLEDeviceConnectStatus_Failure = -100,
    APBLEDeviceConnectStatus_Timeout = -101,
};

///蓝牙指令
typedef NS_ENUM(NSUInteger, APBLEVLinkCommandType) {
    APBLEVLinkCommandType_ConnnectState = 0x10011, ///< 自定义指令，连接状态
    APBLEVLinkCommandType_RemoteUnlock = 0x8001, ///< 远程开锁
    APBLEVLinkCommandType_RemoteLock = 0x8002, ///< 远程关锁
    APBLEVLinkCommandType_RandomCode = 0x8003, ///< 随机码
    APBLEVLinkCommandType_PasswordAdd = 0x8015, ///< 添加密码
    APBLEVLinkCommandType_PasswordDelete = 0x8016, ///< 删除密码
    APBLEVLinkCommandType_PasswordList = 0x8017, ///< 密码列表
    APBLEVLinkCommandType_AdminPasswordSet = 0x8018, ///< 设置管理员密码
    APBLEVLinkCommandType_LanguageGet = 0x8019, ///< 获取语言
    APBLEVLinkCommandType_LanguageSet = 0x8020, ///< 设置语言
    APBLEVLinkCommandType_AdminPasswordGet = 0x8088, ///< 获取管理员密码 废弃了没有用了，改成0x8088，避免跟语言设置获取指令冲突
    APBLEVLinkCommandType_ReportUnlock = 0x8050, ///< 开锁事件上报 *
    APBLEVLinkCommandType_ReportLock = 0x8051, ///< 关锁上报
    APBLEVLinkCommandType_ReportLowPower = 0x8052, ///< 低电量上报
    APBLEVLinkCommandType_ReportLockException = 0x8053, ///< 关锁异常事件
    APBLEVLinkCommandType_ReportUnlockException = 0x8054, ///< 开锁异常上报
    APBLEVLinkCommandType_ReportPwdAddition = 0x8055, ///< 密码添加上报
    APBLEVLinkCommandType_ReportPwdDeletion = 0x8056, ///< 密码删除上报
    APBLEVLinkCommandType_ReportTrialError = 0x8057, ///< 试错报警上报
    APBLEVLinkCommandType_ReportDoorOpened = 0x8058, ///< 门未关上报
    APBLEVLinkCommandType_ReportDoorUnlock = 0x8059, ///< 门未锁上报
    APBLEVLinkCommandType_ReportCareEvent = 0x8060, ///< 上报关怀模式报警事件
    APBLEVLinkCommandType_ReportMultiLock = 0x8061, ///< 批量开、关锁事件上报
    APBLEVLinkCommandType_ReportMultiAlarm = 0x8062, ///<批量警报事件上报
    /** 上报离家模式报警事件 */
    APBLEVLinkCommandType_ReportLeaveHome = 0x8063,
    APBLEVLinkCommandType_Power = 0x8064, ///< 获取电量
    APBLEVLinkCommandType_CareSet = 0x8065, ///< 设置关怀模式
    APBLEVLinkCommandType_CareGet = 0x8066, ///< 获取关怀模式
    APBLEVLinkCommandType_LockVolumeSet = 0x8067, ///< 设置门锁音量
    APBLEVLinkCommandType_LockVolumeGet = 0x8068, ///< 获取门锁音量
    APBLEVLinkCommandType_GetTamperAlarm = 0x8069, ///< 获取防撬开关
    APBLEVLinkCommandType_SetTamperAlarm = 0x807a, ///< 设置防撬开关
    APBLEVLinkCommandType_LockVerion = 0x8070, ///< 获取门锁版本信息
    APBLEVLinkCommandType_LockProductInfo = 0x8071, ///< 锁的产品信息
    APBLEVLinkCommandType_AutoLockSet = 0x8095, ///< 设置自动闭锁
    APBLEVLinkCommandType_AutoLockGet = 0x8096, ///< 获取自动闭锁
    
    APBLEVLinkCommandType_AutoReplySet = 0x805A, ///< 自动回复设置
    APBLEVLinkCommandType_AutoReplyGet = 0x805B, ///< 自动回复获取
    APBLEVLinkCommandType_LockState = 0x80A5, ///< 获取门锁状态
    APBLEVLinkCommandType_GetLLStatus = 0x80A6, ///< 获取门锁和锁舌状态
    APBLEVLinkCommandType_GetDoorStatus = 0x80A7, ///< 获取门磁状态
    APBLEVLinkCommandType_LockAllAttr = 0x80B0, ///< 获取门锁所有属性(不包含密码列表信息)
    ///< ==================================OTA
    APBLEVLinkCommandType_OTARequest = 0xC010, ///< OTA升级前准备
    APBLEVLinkCommandType_OTAFileInfo = 0xC011, ///< OTA文件信息
    APBLEVLinkCommandType_OTAFileOffset = 0xC012, ///< OTA文件偏移
    APBLEVLinkCommandType_OTAFileData = 0xC013, ///< OTA文件数据
    APBLEVLinkCommandType_OTAFinished = 0xC014, ///< OTA升级结束
    ///< ==================================9xxx系列
    APBLEVLinkCommandType_NetConfig = 0x9001,           ///< 配网
    APBLEVLinkCommandType_CheckActivate = 0x9002,       ///< 配网激活状态
    APBLEVLinkCommandType_GetPidDn = 0x9003,           ///< 获取设备PID&DN信息
    APBLEVLinkCommandType_NetDiagnostic = 0x9010,       ///< 网络诊断
    APBLEVLinkCommandType_ReportActivate = 0x9011,      ///< 激活配网上报
    APBLEVLinkCommandType_SyncBindStatus = 0x9012,       ///< 配对结果回复
    APBLEVLinkCommandType_FingerprintAdd = 0x9020,      ///< 添加指纹
    APBLEVLinkCommandType_FingerprintDelete = 0x9021,   ///< 删除指纹
    APBLEVLinkCommandType_FingerprintCancel = 0x9022,   ///< 取消指纹录入
    APBLEVLinkCommandType_FingerprintProgress = 0x9023, ///< 指纹录入进度
    APBLEVLinkCommandType_SyncTime = 0x9040, ///< 同步网络时间
    APBLEVLinkCommandType_NFCAdd = 0x9050, ///< NFC读取
    APBLEVLinkCommandType_NFCCancel = 0x9051, ///< NFC取消录入
    APBLEVLinkCommandType_NFCAddResult = 0x9052, ///< NFC添加结果上报
    APBLEVLinkCommandType_BindDevice = 0x9060, ///< 绑定设备
    APBLEVLinkCommandType_UnbindDevice = 0x9061, ///< 解绑设备
    APBLEVLinkCommandType_SecretCheck = 0x9062, ///< 密钥校验
    APBLEVLinkCommandType_AccessoryMatch = 0x9063, ///< 配件配对
    APBLEVLinkCommandType_AccessoryMatchResult = 0x9064, ///< 配件配对结果上报
    APBLEVLinkCommandType_EmergencyUnlock = 0x9065, ///< 紧急开锁
    APBLEVLinkCommandType_AdjustLock = 0x9066, ///< 门锁校准
    APBLEVLinkCommandType_DoorLockState = 0x9067, ///< 上报门&锁状态  旧：获取门和锁的状态
    APBLEVLinkCommandType_VendorSN = 0x9068, ///< 第三方的SN信息（比如 homekit sn信息）
    APBLEVLinkCommandType_LatchKeepTime = 0x9069, ///< 设置锁舌保持时间
    APBLEVLinkCommandType_SetLatchOrNot = 0x9070, ///< 设置门锁类型
    APBLEVLinkCommandType_KeyBoardSet = 0x9071, ///< 键盘设置
    APBLEVLinkCommandType_BackLightSet = 0x9072, ///< 背光灯设置
    APBLEVLinkCommandType_AttErrRemindSet = 0x9073, ///< 试错提醒设置
    APBLEVLinkCommandType_ReportCalibrate = 0x9074, ///< 上报门锁校准
    APBLEVLinkCommandType_GetKeepTime = 0x9075, ///< 获取锁舌保持事件
    APBLEVLinkCommandType_GetKeyBoardSet = 0x9076, ///< 获取键盘设置
    APBLEVLinkCommandType_GetLatchType = 0x9077, ///< 获取锁类型
    APBLEVLinkCommandType_BackLightGet = 0x9078, ///< 获取背光灯
    APBLEVLinkCommandType_AttErrRemindGet = 0x9079, ///< 获取试错提醒
    APBLEVLinkCommandType_PalmAdd = 0x9080, ///< 添加掌静脉
    APBLEVLinkCommandType_PalmAddCancel = 0x9081, ///< 取消添加掌静脉
    APBLEVLinkCommandType_PalmAddResult = 0x9082, ///< 掌静脉添加结果上报
    APBLEVLinkCommandType_EmrgencyUnlockResult = 0x9083, ///< 上报紧急开锁结果
    APBLEVLinkCommandType_AddFace = 0x9084, ///< 添加人脸
    APBLEVLinkCommandType_CancelAddFace = 0x9085, ///< 取消添加人脸
    APBLEVLinkCommandType_FaceProgressNotify = 0x9086, ///< 人脸添加结果上报
    /** 上报防拆告警事件 */
    APBLEVLinkCommandType_ReportTamperAlarm = 0x9088,
};


