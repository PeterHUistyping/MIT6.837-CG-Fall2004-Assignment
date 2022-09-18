#pragma once
#include "vectors.h"
#include <iostream>
#include <random>
#include <iomanip>
#include <math.h>
class Sampler
{
public:
    Sampler(int _num_samples) { num_samples = _num_samples; }
    virtual ~Sampler() {}
    /**
     * @param  n  sample index (k)
     * @return 2D offset of the sample
     * */
    virtual Vec2f getSamplePosition(int n) = 0;

protected:
    int num_samples;
};
class RandomSampler : public Sampler
{
public:
    RandomSampler(int _num_samples) : Sampler(_num_samples) {}
    ~RandomSampler() {}
    /**
     * @brief generate the normal position for the sample
     * @param  n  sample index (k)
     * @return 2D offset of the sample
     * */
    Vec2f getSamplePosition(int n)
    {
        constexpr float FLOAT_MIN = 0.0;
        const float FLOAT_MAX = 1.0;
        // const float FLOAT_MAX1 = 1.0 / width;
        // const float FLOAT_MAX2 = 1.0 / height;
        std::random_device rd;
        std::default_random_engine eng(rd());
        std::uniform_real_distribution<float> distr(FLOAT_MIN, FLOAT_MAX);
        // std::uniform_real_distribution<float> distr1(FLOAT_MIN, FLOAT_MAX1);
        // std::uniform_real_distribution<float> distr2(FLOAT_MIN, FLOAT_MAX2);
        return Vec2f(distr(eng), distr(eng));
        // return Vec2f(distr1(eng), distr2(eng));
    }
};
class UniformSampler : public Sampler
{
public:
    UniformSampler(int _num_samples) : Sampler(_num_samples) {}
    ~UniformSampler() {}
    /**
     * @param  n  sample index (k)
     * @return 2D offset of the sample
     * */
    Vec2f getSamplePosition(int n)
    {
        int single = (int)sqrt(num_samples);
        int i = n % single, j = n / single;
        float i_ = (i + 1) * 1.0 / (single + 1);
        float j_ = (j + 1) * 1.0 / (single + 1);
        assert(i_ <= 1 && j_ <= 1 && i_ >= 0 && j_ >= 0);
        return Vec2f(i_, j_);
    };
};
class JitteredSampler : public Sampler
{
public:
    JitteredSampler(int _num_samples) : Sampler(_num_samples) {}
    ~JitteredSampler() {}
    /**
     * @param  n  sample index (k)
     * @return 2D offset of the sample
     * */
    Vec2f getSamplePosition(int n)
    {
        // Uniform sample + random perturbation
        int single = (int)sqrt(num_samples);
        int i = n % single, j = n / single;
        float i_ = (i + 1) * 1.0 / (single + 1);
        float j_ = (j + 1) * 1.0 / (single + 1);
        assert(i_ <= 1 && j_ <= 1 && i_ >= 0 && j_ >= 0);

        constexpr float FLOAT_MIN = 0.0;
        const float FLOAT_MAX = 1.0 / (single + 1);
        // const float FLOAT_MAX1 = 1.0 / width;
        // const float FLOAT_MAX2 = 1.0 / height;
        std::random_device rd;
        std::default_random_engine eng(rd());
        std::uniform_real_distribution<float> distr(FLOAT_MIN, FLOAT_MAX);
        // std::uniform_real_distribution<float> distr1(FLOAT_MIN, FLOAT_MAX1);
        // std::uniform_real_distribution<float> distr2(FLOAT_MIN, FLOAT_MAX2);
        return Vec2f(i_ + distr(eng), j_ + distr(eng));
    };
};