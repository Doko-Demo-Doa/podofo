// swift-tools-version:5.7
import PackageDescription

let package = Package(
    name: "PoDoFo",
    platforms: [ .iOS(.v17) ],
    products: [ .library(name: "PoDoFo", targets: ["PoDoFo"]) ],
    targets: [
        .binaryTarget(
            name: "PoDoFo",
            url: "https://github.com/Doko-Demo-Doa/podofo/releases/download/v0.1.2/PoDoFo-0.1.2.xcframework.zip",
            checksum: "482364ccf0a3832f52868c6e0cc3116c4aee44f39ab1fa98a05ff9d8c7801d82"
        )
    ]
)
