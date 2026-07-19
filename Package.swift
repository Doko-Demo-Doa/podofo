// swift-tools-version:5.7
import PackageDescription

let package = Package(
    name: "PoDoFo",
    platforms: [ .iOS(.v17) ],
    products: [ .library(name: "PoDoFo", targets: ["PoDoFo"]) ],
    targets: [
        .binaryTarget(
            name: "PoDoFo",
            url: "https://github.com/Doko-Demo-Doa/podofo/releases/download/v0.0.6/PoDoFo-0.0.6.xcframework.zip",
            checksum: "8f09dd7a9f496eb6afb400bcaff96ec0fdd6772379d0892add1e0594f70407df"
        )
    ]
)
