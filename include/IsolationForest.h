//	MIT License
//
//  Copyright © 2017 Michael J Simms. All rights reserved.
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.

#pragma once

#include <map>
#include <random>
#include <set>
#include <stdint.h>
#include <string>
#include <vector>

namespace IsolationForest
{
    /// This class represents a feature. Each sample has one or more features.
    /// Each feature has a name and value.
    class Feature
    {
    public:
        Feature(const std::string &name, unsigned short value)
        {
            m_name = name;
            m_value = value;
        };
        virtual ~Feature(){};

        virtual void Name(std::string &name) { m_name = name; };
        virtual std::string Name() const { return m_name; };
        virtual void Value(unsigned short value) { m_value = value; };
        virtual unsigned short Value() const { return m_value; };

    protected:
        std::string m_name;
        unsigned short m_value;

    private:
        Feature(){};
    };

    typedef Feature *FeaturePtr;
    typedef std::vector<FeaturePtr> FeaturePtrList;

    /// This class represents a sample.
    /// Each sample has a name and list of features.
    class Sample
    {
    public:
        Sample(){};
        Sample(const std::string &name) { m_name = name; };
        virtual ~Sample(){};

        virtual void AddFeatures(const FeaturePtrList &features) { m_features.insert(std::end(m_features), std::begin(features), std::end(features)); };
        virtual void AddFeature(const FeaturePtr feature) { m_features.push_back(feature); };
        virtual FeaturePtrList Features() const { return m_features; };

    private:
        std::string m_name;
        FeaturePtrList m_features;
    };

    typedef Sample *SamplePtr;
    typedef std::vector<SamplePtr> SamplePtrList;

    /// Tree node, used internally.
    class Node
    {
    public:
        Node();
        Node(const std::string &featureName, unsigned short splitValue);
        virtual ~Node();

        virtual std::string FeatureName() const { return m_featureName; };
        virtual unsigned short SplitValue() const { return m_splitValue; };

        Node *Left() const { return m_left; };
        Node *Right() const { return m_right; };

        void SetLeftSubTree(Node *subtree);
        void SetRightSubTree(Node *subtree);

        std::string Dump() const;

    private:
        std::string m_featureName;
        unsigned short m_splitValue;

        Node *m_left;
        Node *m_right;

        void DestroyLeftSubtree();
        void DestroyRightSubtree();
    };

    typedef Node *NodePtr;
    typedef std::vector<NodePtr> NodePtrList;

    /// This class abstracts the random number generation.
    /// Inherit from this class if you wish to provide your own randomizer.
    /// Use Forest::SetRandomizer to override the default randomizer with one of your choosing.
    class Randomizer
    {
    public:
        Randomizer() : m_gen(m_rand()){};
        virtual ~Randomizer(){};

        virtual unsigned short Rand() { return m_dist(m_gen); };
        virtual unsigned short RandUShort(unsigned short min, unsigned short max) { return min + (Rand() % (max - min + 1)); }

    private:
        std::random_device m_rand;
        std::mt19937_64 m_gen;
        std::uniform_int_distribution<unsigned short> m_dist;
    };

    typedef std::set<unsigned short> UShortSet;
    typedef std::map<std::string, UShortSet> FeatureNameToValuesMap;

    /// Isolation Forest implementation.
    class Forest
    {
    public:
        Forest();
        Forest(unsigned short numTrees, unsigned short subSamplingSize);
        virtual ~Forest();

        void SetRandomizer(Randomizer *newRandomizer);
        void AddSample(const Sample &sample);
        void Create();

        float Score(const Sample &sample);
        float NormalizedScore(const Sample &sample);

        std::string Dump() const;

    private:
        Randomizer *m_randomizer;               // Performs random number generation
        FeatureNameToValuesMap m_featureValues; // Lists each feature and maps it to all unique values in the training set
        NodePtrList m_trees;                    // The decision trees that comprise the forest
        unsigned short m_numTreesToCreate;            // The maximum number of trees to create
        unsigned short m_subSamplingSize;             // The maximum depth of a tree

        NodePtr CreateTree(const FeatureNameToValuesMap &featureValues, size_t depth);
        float Score(const Sample &sample, const NodePtr tree);

        void Destroy();
        void DestroyRandomizer();
    };
};