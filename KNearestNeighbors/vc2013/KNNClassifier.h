#pragma once
#include <vector>

//using KNNClassifierf = KNNClassifier<float>;

template<class T1, class T2>
class KNNClassifier
{
public:
	using FeatureMap = std::map<C, std::vector<std::vector<T2>>>;

	KNNClassifier(const FeatureMap &aTrainingData, uint32_t aK = 3);

	T1 predict(const std::vector<std::vector<T2>> &aTestData) const;
private:
	uint32_t mK;
	FeatureMap mTrainingData;
};



