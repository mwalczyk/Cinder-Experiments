#pragma once
#include "cinder/Log.h"
#include "cinder/app/App.h"
#include "cinder/Rand.h"
#include "cinder/Color.h"
#include "cinder/gl/wrapper.h"
#include "cinder/gl/draw.h"
#include <map>
#include <vector>
#include <algorithm>
#include <numeric>

// reference: http://www.codeproject.com/Articles/48575/How-to-define-a-template-class-in-a-h-file-and-imp

template<class ClassT, class DataT>
class KNNClassifier
{
public:
	using FeatureMap = std::map<ClassT, std::vector<std::vector<DataT>>>;

	KNNClassifier();

	void loadTrainingData(const FeatureMap &aTrainingData, uint8_t aK = 3);
	void drawTrainingData() const;
	ClassT predict(const std::vector<DataT> &aTestItem) const;
private:
	uint8_t mK;
	FeatureMap mTrainingData;
	std::map<ClassT, ci::Color> mPalette;
};

template<class ClassT, class DataT>
KNNClassifier<ClassT, DataT>::KNNClassifier()
{
}

template<class ClassT, class DataT>
void KNNClassifier<ClassT, DataT>::loadTrainingData(const FeatureMap &aTrainingData, uint8_t aK = 3)
{
	if (aTrainingData.size() > aK)
	{
		console() << "K is set to a value less than the total number of voting groups." << std::endl;
	}

	mTrainingData = aTrainingData;
	mK = aK;

	// generate a color palette
	for (const auto& cls : mTrainingData)
	{
		mPalette[cls.first].set(CM_RGB, randVec3());
	}
}

template<class ClassT, class DataT>
void KNNClassifier<ClassT, DataT>::drawTrainingData() const
{
	int w = getWindowWidth();
	int h = getWindowHeight();
	for (const auto& cls : mTrainingData)
	{
		for (const auto& dp : cls.second)
		{
			ci::gl::color(mPalette[cls.first]);
			ci::gl::drawSolidCircle(vec2(dp[0], dp[1]), 4.0f);
		}
	}
}

template<class ClassT, class DataT>
ClassT KNNClassifier<ClassT, DataT>::predict(const std::vector<DataT> &aTestItem) const
{
	// a container to store all of the Euclidean distances, where each item is a pair that contains a class and a distance
	std::vector<std::pair<ClassT, DataT>> distances;

	// a container to hold the vote count for each class
	std::map<ClassT, uint8_t> votes;

	// iterate through all of the classes
	for (const auto& cls : mTrainingData)
	{
		votes[cls.first] = 0;

		// iterate through the vector of data points (each of which is a vector) corresponding to this class
		for (const auto& dp : cls.second)
		{
			std::vector<DataT> sumOfSquares(dp.size());
			std::transform(dp.begin(), dp.end(), aTestItem.begin(), sumOfSquares.begin(), [&](DataT lhs, DataT rhs) {
				return (rhs - lhs) * (rhs - lhs);
			});

			DataT sum = std::accumulate(sumOfSquares.begin(), sumOfSquares.end(), 0);
			double dist = sqrt(static_cast<double>(sum));

			// store the class associated with this distance
			distances.emplace_back(std::make_pair(cls.first, dist));
		}
	}

	// sort in ascending order, based on distance
	std::sort(distances.begin(), distances.end(), [](std::pair<ClassT, DataT> lhs, std::pair<ClassT, DataT> rhs) { 
		return lhs.second < rhs.second; 
	});

	// iterate through the k-closest data points and find the class with the higher number of votes
	for (auto it = distances.begin(); it < distances.begin() + mK; ++it)
	{
		++votes[(*it).first];
		console() << "Class: " << (*it).first << ", Distance: " << (*it).second << std::endl;
	}

	// find the most common class among the k-closest data points
	auto mostCommon = std::max_element(votes.begin(), votes.end(), [](std::map<ClassT, uint8_t>::value_type lhs, std::map<ClassT, uint8_t>::value_type rhs) {
		return lhs.second < rhs.second;
	});
	console() << "Highest: " << (*mostCommon).first << std::endl;
	return (*mostCommon).first;
}



