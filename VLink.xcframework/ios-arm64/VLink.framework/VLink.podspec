#
#  Be sure to run `pod spec lint LDLogReportModule.podspec' to ensure this is a
#  valid spec and to remove all comments including this before submitting the spec.
#
#  To learn more about Podspec attributes see https://guides.cocoapods.org/syntax/podspec.html
#  To see working Podspecs in the CocoaPods repo see https://github.com/CocoaPods/Specs/
#

Pod::Spec.new do |s|

# ―――  Spec Metadata  ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――― #
#
#  These will help people to find your library, and whilst it
#  can feel like a chore to fill in it's definitely to your advantage. The
#  summary should be tweet-length, and the description more in depth.
#


s.name         = "VLink"
s.version      = "1.0.0"
s.summary      = "凌度智能门锁蓝牙连接库"


s.description  = <<-DESC
凌度智能门锁蓝牙连接库，支持智能门锁设备的网络配置，设备设置、OTA升级等功能的蓝牙通讯协议。
DESC

s.homepage     = "http://192.168.2.3:3000/fuqingping/VLink-iOS"

s.author             = { "fuqingping" => "championfu1990@gmail.com" }
s.platform     = :ios, "13.0"
s.requires_arc = true

s.source       = { :git => "http://192.168.2.3:3000/fuqingping/VLink-iOS.git", :branch => "no-log" }

s.source_files = 'VLink/Classes/**/*.{h,m}'

s.public_header_files = "VLink/Classes/Headers/*.h"

# s.pod_target_xcconfig = { 'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'arm64' }

s.frameworks = 'Foundation', 'Security', 'CoreBluetooth', 'QuartzCore'

end
