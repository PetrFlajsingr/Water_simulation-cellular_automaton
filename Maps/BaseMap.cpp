//
// Created by Igor Frank on 06.01.20.
//

#include "BaseMap.h"
#include "WaterfallMap.h"
#include <BasicBowlMap.h>
std::unique_ptr<BaseMap> BaseMap::CreateInstance(MapType type) {
  switch (type) {
  case MapType::BasicBowl:
    return std::make_unique<BasicBowlMap>();
  case MapType::Waterfall:
    return std::make_unique<WaterfallMap>();
  }
}
