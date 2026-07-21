// swift-tools-version:5.7
import PackageDescription

let package = Package(
    name: "PoDoFo",
    platforms: [ .iOS(.v17) ],
    products: [ .library(name: "PoDoFo", targets: ["PoDoFo"]) ],
    targets: [
        .binaryTarget(
            name: "PoDoFo",
            url: "https://github.com/Doko-Demo-Doa/podofo/releases/download/v0.0.11/PoDoFo-0.0.11.xcframework.zip",
            checksum: "3cdabc647ce0cab551a01de784c6daf6f2e72ff30268281993d051e76836dfc2"
        )
    ]
)
