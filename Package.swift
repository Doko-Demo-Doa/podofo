// swift-tools-version:5.7
import PackageDescription

let package = Package(
    name: "PoDoFo",
    platforms: [ .iOS(.v17) ],
    products: [ .library(name: "PoDoFo", targets: ["PoDoFo"]) ],
    targets: [
        .binaryTarget(
            name: "PoDoFo",
            url: "https://github.com/Doko-Demo-Doa/podofo/releases/download/v0.1.1/PoDoFo-0.1.1.xcframework.zip",
            checksum: "5415cc05355d60d4d7e05a45f8d76f20cd124704a10075678e6962c6c4ec16f5"
        )
    ]
)
