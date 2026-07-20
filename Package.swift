// swift-tools-version:5.7
import PackageDescription

let package = Package(
    name: "PoDoFo",
    platforms: [ .iOS(.v17) ],
    products: [ .library(name: "PoDoFo", targets: ["PoDoFo"]) ],
    targets: [
        .binaryTarget(
            name: "PoDoFo",
            url: "https://github.com/Doko-Demo-Doa/podofo/releases/download/v0.0.8/PoDoFo-0.0.8.xcframework.zip",
            checksum: "3b208932ecfca4f17aae5660905911a40ca765c749a0da20cda4d807636b8bb3"
        )
    ]
)
