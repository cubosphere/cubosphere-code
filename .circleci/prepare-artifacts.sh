#!/bin/bash
mkdir -p /tmp/artifacts
mv build/*.deb /tmp/artifacts
mv build/*.tar.gz /tmp/artifacts || true
