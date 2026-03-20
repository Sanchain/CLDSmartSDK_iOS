Pod::Spec.new do |s|
  s.name        = "CLDSmartSDK_iOS"
  s.version     = "1.0.0"
  s.summary     = "集成FuweiCloud接口、音视频模块、蓝牙模块功能的SDK"
  s.homepage    = "http://gitlab.lindu-lab.com/hongtao.liu/cldsmartsdk_ios"
  s.license     = { :type => "MIT" }
  s.authors     = { "Champion Fu" => "championfu1990@gmail.com"  }
  s.swift_version = "5.9"
  s.requires_arc = true
  s.platform      = :ios
  s.ios.deployment_target = '13.0'
  #s.source_files = 'CLDSmartSDK/Classes/**/*.swift'
  s.source   = { :git => "ssh://git@gitlab.lindu-lab.com:8022/hongtao.liu/cldsmartsdk_ios.git" }
  s.vendored_frameworks = "CLDSmartSDK.xcframework", "VLink.xcframework"
  s.dependency "CocoaMQTT"
  s.dependency "AgoraRtcEngine_iOS"
  #s.ios.dependency 'AgoraRtcEngine_iOS/RtcBasic', '4.3.2'
  #s.ios.dependency 'AgoraRtcEngine_iOS/AINS', '4.3.2'
  #s.ios.dependency 'AgoraRtcEngine_iOS/AIAEC', '4.3.2'
  #s.ios.dependency 'AgoraRtcEngine_iOS/ClearVision', '4.3.2'
  #s.ios.dependency 'AgoraRtcEngine_iOS/VideoCodecDec', '4.3.2'
  #s.ios.dependency 'AgoraRtcEngine_iOS/VideoAv1CodecDec', '4.3.2'
  #s.ios.dependency 'AgoraRtcEngine_iOS/AudioBeauty', '4.3.2'
  #s.frameworks = 'UIKit', 'Security'
  
end
