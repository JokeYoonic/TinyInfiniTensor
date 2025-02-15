#include "operators/matmul.h"

namespace infini
{

    MatmulObj::MatmulObj(GraphObj *graph, Tensor A, Tensor B, Tensor C, bool transA,
                         bool transB)
        : OperatorObj(OpType::MatMul, TensorVec{A, B}, {C}),
          transA(transA), transB(transB)
    {
        IT_ASSERT(checkValid(graph));
    }

    string MatmulObj::toString() const
    {
        std::ostringstream os;
        os << "Matmul([" << (transA ? "A^T" : "A") << "," << (transB ? "B^T" : "B]")
           << ",A=" << inputs[0]->getGuid()
           << ",B=" << inputs[1]->getGuid() << ",C=" << outputs[0]->getGuid()
           << ",mnk=[" << m << "," << n << "," << k << "])";
        return os.str();
    }

    optional<vector<Shape>> MatmulObj::inferShape(const TensorVec &inputs)
    {
        IT_ASSERT(inputs.size() == 2);
        const auto& A = inputs[0];
        const auto& B = inputs[1];

        auto aDims = A->getDims();
        auto bDims = B->getDims();

        IT_ASSERT(aDims.size() == 2 && bDims.size() == 2);

        int m = static_cast<int>(transA ? aDims[1] : aDims[0]);
        int k1 = static_cast<int>(transA ? aDims[0] : aDims[1]);
        int k2 = static_cast<int>(transB ? bDims[1] : bDims[0]);
        int n = static_cast<int>(transB ? bDims[0] : bDims[1]);

        IT_ASSERT(k1 == k2);

        return vector<Shape>{{m, n}};
    }
    void MatmulObj::setTransposeA(bool transA) {
        this->transA = transA;
    }
} // namespace infini