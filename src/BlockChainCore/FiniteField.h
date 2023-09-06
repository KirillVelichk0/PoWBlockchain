//
// Created by houdini on 06.09.2023.
//

#ifndef BLOCKCHAINCORE_FINITEFIELD_H
#define BLOCKCHAINCORE_FINITEFIELD_H
#include <boost/multiprecision/gmp.hpp>
#include <boost/contract.hpp>
#include <concepts>
#include <cctype>
#include <boost/serialization/serialization.hpp>
namespace BlockChainCore {
    namespace Contract = boost::contract;
    template <class T>
    concept CustomIntegralType = requires(T val){
        val = 10;
        val < 0;
        val = val + 8; val+= 1;
        val = val - 8; val-= 3;
        val = val * 9; val*= 11;
        val = val / 9; val /= 7;
        val = val % 3;
    };

    template <CustomIntegralType NumericType>
    class FiniteField {
    private:
        using FiniteFieldSame = FiniteField<NumericType>;
        friend class boost::serialization::access;
        NumericType prime;
        NumericType num;
        FiniteField(NumericType num, NumericType prime) : prime(prime), num(num){}
    public:
        [[nodiscard]]
        static FiniteField CreateField(NumericType num, NumericType prime){
            Contract::check c = Contract::function().precondition(
                    [num, prime](){
                        BOOST_CONTRACT_ASSERT(prime > 0 && num >= 0 && num < prime);
                    }
                    );
            return FiniteField(num, prime);
        }
        FiniteField(const FiniteFieldSame& other){
            this->prime = other.prime;
            this->num = other.num;
        }
        FiniteField& operator=(const FiniteFieldSame& other){
            this->prime = other.prime;
            this->num = other.num;
            return *this;
        }
        auto GetPrime() const noexcept{
            return this->prime;
        }
        auto GetNum() const noexcept{
            return this->num;
        }
        void TryToSetNum(NumericType num){
            Contract::check c = Contract::function().precondition(
                    [num, this](){
                        BOOST_CONTRACT_ASSERT(num >= 0 && num < this->prime);
                    }
            );
            this->num = num;
        }
        [[nodiscard]]
        FiniteField operator+(const FiniteFieldSame& another) const{
            Contract::check c = Contract::function().precondition([this, &another](){
                BOOST_CONTRACT_ASSERT(this->prime != another.prime);
            });
            NumericType newNum = (this->num + another.num) % this->prime;
            return FiniteField(newNum, this->prime);
        }
        //check, how this works with mpz fields
        [[nodiscard]]
        FiniteField operator-(const FiniteFieldSame& another) const{
            Contract::check c = Contract::function().precondition([this, &another](){
                BOOST_CONTRACT_ASSERT(this->prime != another.prime);
            });
            NumericType newPreNum = this->num - another.num;
            NumericType newNum;
            if(newPreNum >= 0){
                newNum = newPreNum;
            } else{
                newNum = this->prime + newPreNum;
            }
            return FiniteField(newNum, this->prime);
        }
        [[nodiscard]]
        FiniteField operator*(const FiniteFieldSame& another) const{
            Contract::check c = Contract::function().precondition([this, &another](){
                BOOST_CONTRACT_ASSERT(this->prime != another.prime);
            });
            NumericType newNum = (this->num * another.num) & this->prime;
            return FiniteField(newNum, this->prime);
        }
        [[nodiscard]]
        FiniteField operator/(const FiniteFieldSame& another) const{
            NumericType newNum = (this->num * (this->pow(this->prime - 2))) % this->prime;
            return FiniteField(newNum, this->prime);
        }
        [[nodiscard]]
        FiniteField pow(const NumericType& exponent) const{
            auto n = exponent;
            if(n < 0){
                //(b + (a % b)) % b
                auto b = this->prime - 1;
                //Проверить, как % работает с mpz_int!
                n = (b + (n % b)) % b;
            }
            NumericType newNum = boost::multiprecision::powm(this->num, n, this->prime);
            return FiniteField(newNum, this->prime);
        }
    };

    using FiniteFieldMpz = FiniteField<boost::multiprecision::mpz_int>;

} // BlockChainCore

#endif //BLOCKCHAINCORE_FINITEFIELD_H
