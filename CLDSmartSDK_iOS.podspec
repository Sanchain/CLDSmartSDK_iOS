Pod::Spec.new do |s|
  s.name        = "CLDSmartSDK_iOS"
  s.version     = "1.4.12"
  s.summary     = "集成FuweiCloud接口、音视频模块、蓝牙模块功能的SDK"
  s.homepage    = "https://github.com/Sanchain/CLDSmartSDK_iOS"
  s.license     = { :type => "MIT" }
  s.authors     = { "Champion Fu" => "championfu1990@gmail.com"  }
  s.swift_version = "5.9"
  s.requires_arc = true
  s.platform      = :ios
  s.ios.deployment_target = '13.0'
  #s.source_files = 'CLDSmartSDK/Classes/**/*.swift'
  s.source   = { :git => "https://github.com/Sanchain/CLDSmartSDK_iOS.git", :tag => s.version.to_s }
  # Use the exact CocoaMQTT binary that CLDSmartSDK was built against.
  # Mixing it with a separately compiled CocoaPods release causes Swift ABI link errors.
  s.vendored_frameworks = "CLDSmartSDK.xcframework", "VLink.xcframework", "CocoaMQTT.xcframework"
  s.dependency "AgoraRtcEngine_iOS", '4.3.0'
  #s.ios.dependency 'AgoraRtcEngine_iOS/RtcBasic', '4.3.2'
  #s.ios.dependency 'AgoraRtcEngine_iOS/AINS', '4.3.2'
  #s.ios.dependency 'AgoraRtcEngine_iOS/AIAEC', '4.3.2'
  #s.ios.dependency 'AgoraRtcEngine_iOS/ClearVision', '4.3.2'
  #s.ios.dependency 'AgoraRtcEngine_iOS/VideoCodecDec', '4.3.2'
  #s.ios.dependency 'AgoraRtcEngine_iOS/VideoAv1CodecDec', '4.3.2'
  #s.ios.dependency 'AgoraRtcEngine_iOS/AudioBeauty', '4.3.2'
  #s.frameworks = 'UIKit', 'Security'
  
end
