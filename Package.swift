// swift-tools-version:5.7
import PackageDescription

let package = Package(
    name: "PoDoFo",
    platforms: [ .iOS(.v17) ],
    products: [ .library(name: "PoDoFo", targets: ["PoDoFo"]) ],
    targets: [
        .binaryTarget(
            name: "PoDoFo",
            url: "https://github.com/Doko-Demo-Doa/podofo/releases/download/v0.0.15/PoDoFo-0.0.15.xcframework.zip",
            checksum: "aec57f60a9a7014a431ed367d9751455c7d74d3b69f6c5654268ae4703231df3"
        )
    ]
)
