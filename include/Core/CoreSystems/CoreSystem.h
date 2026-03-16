#pragma once

namespace Core
{
    class CoreSystem
    {
    public:
        CoreSystem() = default;
        virtual ~CoreSystem() = default;

        virtual void Initialize(){}
        virtual void Update(){}
    };
}