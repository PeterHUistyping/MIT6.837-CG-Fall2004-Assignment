#pragma once

class Filter
{
public:
    Filter() {}
    virtual ~Filter() {}
    Vec3f getColor(int i, int j, Film *film)
    {
        Vec3f Color(0, 0, 0);
        float sum_of_weight = 0;
        float current_weight = 0;
        for (int i1 = i - getSupportRadius(); i1 <= i + getSupportRadius(); i1++)
        {
            for (int j1 = j - getSupportRadius(); j1 <= j + getSupportRadius(); j1++)
            {
                // assert(i1 >= 0 && j1 >= 0 && i1 <= film->getWidth() && j1 <= film->getHeight());
                if (i1 < 0 || j1 < 0 || i1 >= film->getWidth() || j1 >= film->getHeight())
                    continue;
                for (int k = 0; k < film->getNumSamples(); k++)
                {
                    float x = film->getSample(i1, j1, k).getPosition().x();
                    float y = film->getSample(i1, j1, k).getPosition().y();
                    current_weight = getWeight(x + i1 - (i + 0.5), y + j1 - (j + 0.5)); // relative to center
                    sum_of_weight += current_weight;
                    Color += current_weight * film->getSample(i1, j1, k).getColor();
                }
            }
        }
        assert(sum_of_weight != 0);
        Color /= sum_of_weight;
        return Color;
    }
    /**
     * @brief In general, this function will have maximum value at the pixel center (when x=0 & y=0)
     * @return  weight for point (x + pixel center, y + pixel center).
     * */
    virtual float getWeight(float x, float y) = 0;
    virtual int getSupportRadius() = 0;
};

class BoxFilter : public Filter
{
public:
    BoxFilter(float radius) { radius_ = radius; }
    ~BoxFilter() {}
    float getWeight(float x, float y)
    {
        // if (x >= -radius_ && y >= -radius_ && x <= radius_ && y <= radius_)
        if (fabs(x) <= radius_ && fabs(y) <= radius_) // symmetric
        {
            return 1;
        }
        return 0;
    }
    int getSupportRadius()
    {
        return int(radius_ + 0.499999);
        // <=0.5 0 ; >0.5 1
    }

private:
    float radius_;
};

class TentFilter : public Filter
{
public:
    TentFilter(float radius) { radius_ = radius; }
    ~TentFilter() {}
    float getWeight(float x, float y)
    {
        float d = sqrt(x * x + y * y);
        if (d > radius_)
        {
            return 0;
        }
        return 1 - d / radius_; // linear function
    }
    int getSupportRadius()
    {
        return int(radius_ + 0.499999);
        // <=0.5 0 ; >0.5 1}
    }

private:
    float radius_;
};
class GaussianFilter : public Filter
{
public:
    GaussianFilter(float sigma) { sigma_ = sigma; }
    ~GaussianFilter() {}
    float getWeight(float x, float y)
    {
        float d = x * x + y * y;
        return exp(-d / 2 / sigma_ / sigma_);
    }
    int getSupportRadius() { return (int)(2 * sigma_); }

private:
    float sigma_;
};
