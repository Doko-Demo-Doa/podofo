// swift-tools-version:5.7
import PackageDescription

let package = Package(
    name: "PoDoFo",
    platforms: [ .iOS(.v17) ],
    products: [ .library(name: "PoDoFo", targets: ["PoDoFo"]) ],
    targets: [
        .binaryTarget(
            name: "PoDoFo",
            url: "https://github.com/Doko-Demo-Doa/podofo/releases/download/v0.0.4/PoDoFo-0.0.4.xcframework.zip",
            checksum: "88ea5f5d55bc8c0ba4200cc088d6037ff9d352c5e57d1f97d3155131ea664d5a"
        )
    ]
)
