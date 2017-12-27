cmake_minimum_required(VERSION 3.9)
project(inter)

set(CMAKE_CXX_STANDARD 11)

add_executable(RangeScanOO main.cpp ActiveEdge.h ActivePolygon.h
        Edge.h Polygon.h Polygon.h RangeInter.h)
