// swift-tools-version:5.7
import PackageDescription

let package = Package(
    name: "PoDoFo",
    platforms: [ .iOS(.v17) ],
    products: [ .library(name: "PoDoFo", targets: ["PoDoFo"]) ],
    targets: [
        .binaryTarget(
            name: "PoDoFo",
            url: "https://github.com/Doko-Demo-Doa/podofo/releases/download/v0.0.16/PoDoFo-0.0.16.xcframework.zip",
            checksum: "8fac3d72eb2e5f16c1d9b4c4a525a14a7bd53df445ee22c4f9c976808334b0bc"
        )
    ]
)
