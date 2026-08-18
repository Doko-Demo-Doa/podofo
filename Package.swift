// swift-tools-version:5.7
import PackageDescription

let package = Package(
    name: "PoDoFo",
    platforms: [ .iOS(.v17) ],
    products: [ .library(name: "PoDoFo", targets: ["PoDoFo"]) ],
    targets: [
        .binaryTarget(
            name: "PoDoFo",
            url: "https://github.com/Doko-Demo-Doa/podofo/releases/download/v0.0.17/PoDoFo-0.0.17.xcframework.zip",
            checksum: "87a3e037b8537f7665ab47208a32c7ddc223cf9710f66fe61a2d5827eed6d756"
        )
    ]
)
