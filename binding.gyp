{
  "targets": [
    {
      "target_name": "addon",
      "sources": [ "cpp/Addon.cc", "cpp/StlMesh.cpp", "cpp/StlMesh.hpp" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
