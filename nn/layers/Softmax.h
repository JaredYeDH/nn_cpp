/**
 * @file Softmax.h
 *
 * @breif A Softmax layer
 *
 * @date 12/17/17
 * @author Ben Caine
 */
#ifndef NN_CPP_SOFTMAX_H
#define NN_CPP_SOFTMAX_H

#include "layers/Layer.h"

namespace nn {
    template <typename Dtype = float, int Dims = 2>
    class Softmax : public Layer<Dtype, Dims> {
    public:

        /**
         * @brief initialize Softmax
         */
        Softmax() = default;

        /**
         * @brief Return the name of the layer
         * @return The layer name
         */
        const std::string& getName() {
            const static std::string name = "Softmax";
            return name;
        }

        /**
         * @brief Forward through the layer (compute the output)
         * @param input [in]: The input tensor to apply softmax to
         * @return
         */
        Eigen::Tensor<Dtype, Dims> forward(const Eigen::Tensor<Dtype, Dims> &input);

        /**
         * @brief Compute the gradient (backwards pass) of the layer
         * @param input [in]: The input tensor to the backwards pass (from the next layer). This should be one hot encoded labels
         * @return The output of the backwards pass (sent ot the previous layer)
         */
        Eigen::Tensor<Dtype, Dims> backward(const Eigen::Tensor<Dtype, Dims> &input);

        void printOutputShape() {}

    private:
        Eigen::Tensor<Dtype, Dims> m_output; ///< The output of the forward pass
    };

    template <typename Dtype, int Dims>
    Eigen::Tensor<Dtype, Dims> Softmax<Dtype, Dims>::forward(const Eigen::Tensor<Dtype, Dims> &input) {
        int batchSize = input.dimensions()[0];
        int classDims = input.dimensions()[1];
        auto shiftedInput = input - input.maximum(Eigen::array<int, 1>{1})
                                    .eval().reshape(Eigen::array<int, 2>{batchSize, 1})
                                    .broadcast(Eigen::array<int, 2>{1, classDims});

        auto exponentiated = shiftedInput.exp();
        m_output = exponentiated * exponentiated.sum(Eigen::array<int, 1>{1})
                                   .inverse().eval()
                                   .reshape(Eigen::array<int, 2>({batchSize, 1}))
                                   .broadcast(Eigen::array<int, 2>({1, classDims}));
        return m_output;
    }

    template <typename Dtype, int Dims>
    Eigen::Tensor<Dtype, Dims> Softmax<Dtype, Dims>::backward(const Eigen::Tensor<Dtype, Dims> &input) {
        int batchSize = input.dimensions()[0];
        // Input will be labels
        assert(batchSize == m_output.dimensions()[0]), "Dimensions of number of batches does not match";
        return (m_output * (input * input.constant(-1))) / input.constant(batchSize);
    }
}

#endif //NN_CPP_SOFTMAX_H
