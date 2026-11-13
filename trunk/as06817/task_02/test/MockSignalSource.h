#pragma once

#include <gmock/gmock.h>

#include "ISignalSource.h"

class MockSignalSource : public ISignalSource {
public:
    MOCK_METHOD(double, value, (int tau), (const, override));
};
