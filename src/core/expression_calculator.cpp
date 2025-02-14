﻿#include <sstream>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <list>
#include <cmath>
#include <any>
#include <regex>

#include "aris/core/expression_calculator.hpp"
#include "aris/core/reflection.hpp"

namespace aris::core
{
	auto s_mm(Size m, Size n, Size k, const double* A, Size lda, const double* B, Size ldb, double *C, Size ldc) noexcept->void
	{
		for (Size i = 0; i < m; ++i)
		{
			Size row_idx = i * lda;
			for (Size j = 0; j < n; ++j)
			{
				Size idx = i * ldc + j;

				C[idx] = 0;
				for (Size u = 0; u < k; ++u)
				{
					C[idx] += A[row_idx + u] * B[j + u * ldb];
				}
			}
		}
	}
	auto s_mmTN(Size m, Size n, Size k, const double* A, Size lda, const double* B, Size ldb, double *C, Size ldc) noexcept->void
	{
		for (Size i = 0; i < m; ++i)
		{
			for (Size j = 0; j < n; ++j)
			{
				Size idx = i * ldc + j;

				C[idx] = 0;
				for (Size u = 0; u < k; ++u)
				{
					C[idx] += A[i + u * lda] * B[j + u * ldb];
				}
			}
		}
	}
	auto s_mmNT(Size m, Size n, Size k, const double* A, Size lda, const double* B, Size ldb, double *C, Size ldc) noexcept->void
	{
		for (Size i = 0; i < m; ++i)
		{
			Size row_idx = i * lda;
			for (Size j = 0; j < n; ++j)
			{
				Size col_idx = j * ldb;

				Size idx = i * ldc + j;

				C[idx] = 0;
				for (Size u = 0; u < k; ++u)
				{
					C[idx] += A[row_idx + u] * B[col_idx + u];
				}
			}
		}
	}
	auto s_mmTT(Size m, Size n, Size k, const double* A, Size lda, const double* B, Size ldb, double *C, Size ldc) noexcept->void
	{
		for (Size i = 0; i < m; ++i)
		{
			for (Size j = 0; j < n; ++j)
			{
				Size col_idx = j * ldb;
				Size idx = i * ldc + j;

				C[idx] = 0;
				for (Size u = 0; u < k; ++u)
				{
					C[idx] += A[i + u * lda] * B[col_idx + u];
				}
			}
		}
	}


	struct Matrix::Imp
	{
		Size m_, n_;
		bool is_row_major_;
		std::vector<double> data_vec_;
	};
	auto Matrix::empty() const->bool { return imp_->data_vec_.empty(); }
	auto Matrix::size() const->Size { return m()*n(); }
	auto Matrix::data()->double * { return imp_->data_vec_.data(); }
	auto Matrix::data() const->const double * { return imp_->data_vec_.data(); }
	auto Matrix::begin() ->double * { return data(); }
	auto Matrix::begin() const ->const double * { return data(); }
	auto Matrix::end() ->double * { return data() + size(); }
	auto Matrix::end()  const ->const double * { return data() + size(); }
	auto Matrix::m() const->Size { return imp_->m_; }
	auto Matrix::n() const->Size { return imp_->n_; }
	auto Matrix::copySubMatrixTo(const Matrix &subMat, Size beginRow, Size beginCol)->void { copySubMatrixTo(subMat, beginRow, beginCol, subMat.m(), subMat.n()); }
	auto Matrix::toDouble() const->double { return data()[0]; }
	auto Matrix::dsp() const ->void { std::cout << this->toString(); }
	auto Matrix::operator()(Size i, Size j)->double & { return imp_->is_row_major_ ? data()[i*n() + j] : data()[j*m() + i]; }
	auto Matrix::operator()(Size i, Size j) const->const double & { return imp_->is_row_major_ ? data()[i*n() + j] : data()[j*m() + i]; }
	auto Matrix::swap(Matrix &other)->Matrix&
	{
		std::swap(this->imp_->m_, other.imp_->m_);
		std::swap(this->imp_->n_, other.imp_->n_);
		std::swap(this->imp_->is_row_major_, other.imp_->is_row_major_);
		std::swap(this->imp_->data_vec_, other.imp_->data_vec_);

		return *this;
	}
	auto Matrix::resize(Size m, Size n)->Matrix &
	{
		imp_->m_ = m;
		imp_->n_ = n;
		imp_->data_vec_.resize(m*n);
		return *this;
	}
	auto Matrix::transpose()->Matrix &
	{
		imp_->is_row_major_ = !imp_->is_row_major_;
		std::swap(imp_->m_, imp_->n_);
		return *this;
	}
	auto Matrix::copySubMatrixTo(const Matrix &subMat, Size beginRow, Size beginCol, Size rowNum, Size colNum)->void
	{
		if ((beginRow + subMat.m() > m()) || (beginCol + subMat.n() > n()))
		{
			THROW_FILE_LINE("Function CopySubMatrixTo must have subMat smaller than self matrix");
		}

		for (Size i = 0; i < subMat.m(); ++i)
		{
			for (Size j = 0; j < subMat.n(); ++j)
			{
				this->operator()(i + beginRow, j + beginCol) = subMat(i, j);
			}
		}

	}
	auto Matrix::toString() const->std::string
	{
		std::stringstream stream;

		stream.precision(15);
		stream << "{";
		for (Size i = 0; i < m(); ++i)
		{
			for (Size j = 0; j < n(); ++j)
			{
				stream << this->operator()(i, j);
				if (j<n() - 1)stream << ",";
			}
			if (i<m() - 1)
				stream << ";\n";
		}
		stream << "}";

		return stream.str();
	}
	Matrix::~Matrix() = default;
	Matrix::Matrix(Size m, Size n, double value)
	{
		imp_->m_ = m;
		imp_->n_ = n;
		imp_->is_row_major_ = true;
		imp_->data_vec_ = std::vector<double>(m*n, value);
	}
	Matrix::Matrix(Size m, Size n, const double *data) : Matrix(m, n) { std::copy(data, data + m * n, this->data()); }
	Matrix::Matrix(double value) : Matrix(1, 1, value) {}
	Matrix::Matrix(const std::initializer_list<Matrix> &data) : Matrix()
	{
		std::list<std::list<Matrix> > mat_list_list;

		bool need_new_row = true;
		for (auto &i : data)
		{
			if (i.empty())
			{
				need_new_row = true;
			}
			else
			{
				if (need_new_row)
				{
					mat_list_list.push_back(std::list<Matrix>());
					need_new_row = false;
				}

				mat_list_list.back().push_back(i);
			}
		}


		(*this) = aris::core::combineMatrices(mat_list_list);
	}
	Matrix::Matrix() :Matrix(0, 0) {};
	Matrix::Matrix(const Matrix &other) = default;
	Matrix::Matrix(Matrix &&other) { this->swap(other); }
	Matrix& Matrix::operator=(Matrix other) { this->swap(other); return *this; }

	Matrix operator + (const Matrix &m1, const Matrix &m2)
	{
		Matrix ret;

		if (m1.size() == 1)
		{
			ret = Matrix(m2);

			for (auto &d : ret)
			{
				d += *m1.data();
			}
		}
		else if (m2.size() == 1)
		{
			ret = Matrix(m1);

			for (auto &d : ret)
			{
				d += *m2.data();
			}
		}
		else if ((m1.m() == m2.m()) && (m1.n() == m2.n()))
		{
			ret.resize(m1.m(), m1.n());

			for (Size i = 0; i < ret.m(); ++i)
			{
				for (Size j = 0; j < ret.n(); ++j)
				{
					ret(i, j) = m1(i, j) + m2(i, j);
				}
			}
		}
		else
		{
			THROW_FILE_LINE("Can't plus matrices, the dimensions are not equal");
		}
		return ret;
	}
	Matrix operator - (const Matrix &m1, const Matrix &m2)
	{
		Matrix ret;

		if ((m1.m() == 1) && (m1.n() == 1))
		{
			ret = Matrix(m2);

			for (Size i = 0; i < ret.size(); ++i)
			{
				ret.data()[i] = m1(0, 0) - ret.data()[i];
			}
		}
		else if ((m2.m() == 1) && (m2.n() == 1))
		{
			ret = Matrix(m1);

			for (Size i = 0; i < ret.size(); ++i)
			{
				ret.data()[i] -= m2(0, 0);
			}
		}
		else if ((m1.m() == m2.m()) && (m1.n() == m2.n()))
		{
			ret.resize(m1.m(), m1.n());

			for (Size i = 0; i < ret.m(); i++)
			{
				for (Size j = 0; j < ret.n(); j++)
				{
					ret(i, j) = m1(i, j) - m2(i, j);
				}
			}
		}
		else
		{
			THROW_FILE_LINE("Can't minus matrices, the dimensions are not equal");
		}

		return ret;
	}
	Matrix operator * (const Matrix &m1, const Matrix &m2)
	{
		Matrix ret;

		if ((m1.m() == 1) && (m1.n() == 1))
		{
			ret = Matrix(m2);

			for (Size i = 0; i < ret.size(); ++i)
			{
				ret.data()[i] *= m1(0, 0);
			}
		}
		else if ((m2.m() == 1) && (m2.n() == 1))
		{
			ret = Matrix(m1);

			for (Size i = 0; i < ret.size(); ++i)
			{
				ret.data()[i] *= m2(0, 0);
			}
		}
		else if (m1.n() == m2.m())
		{
			ret.resize(m1.m(), m2.n());

			if (m1.imp_->is_row_major_)
			{
				if (m2.imp_->is_row_major_)
				{
					aris::core::s_mm(m1.m(), m2.n(), m1.n(), m1.data(), 1, m2.data(), 1, ret.data(), 1);
				}
				else
				{
					aris::core::s_mmNT(m1.m(), m2.n(), m1.n(), m1.data(), 1, m2.data(), 1, ret.data(), 1);
				}
			}
			else
			{
				if (m2.imp_->is_row_major_)
				{
					aris::core::s_mmTN(m1.m(), m2.n(), m1.n(), m1.data(), 1, m2.data(), 1, ret.data(), 1);
				}
				else
				{
					aris::core::s_mmTT(m1.m(), m2.n(), m1.n(), m1.data(), 1, m2.data(), 1, ret.data(), 1);
				}
			}


		}
		else
		{
			THROW_FILE_LINE("Can't multiply matrices, the dimensions are not equal");
		}

		return ret;
	}
	Matrix operator / (const Matrix &m1, const Matrix &m2)
	{
		Matrix ret;

		if ((m1.m() == 1) && (m1.n() == 1))
		{
			ret = Matrix(m2);

			for (Size i = 0; i < ret.size(); ++i)
			{
				ret.data()[i] = m1(0, 0) / ret.data()[i];
			}
		}
		else if ((m2.m() == 1) && (m2.n() == 1))
		{
			ret = Matrix(m1);

			for (Size i = 0; i < ret.size(); ++i)
			{
				ret.data()[i] /= m2(0, 0);
			}
		}
		else
		{
			THROW_FILE_LINE("Right now, divide operator of matrices is not added");
		}

		return ret;
	}
	Matrix operator - (const Matrix &m1)
	{
		Matrix m(m1.m(), m1.n());
		for (Size i = 0; i < m1.size(); ++i)m.data()[i] = -m1.data()[i];
		return m;
	}
	Matrix operator + (const Matrix &m1)
	{
		return m1;
	}

	struct Calculator::Imp
	{
		struct Value
		{
			std::string type_;
			std::any value_;

			auto val()->std::any& { return std::any_cast<void*>(&value_) ? *reinterpret_cast<std::any*>(std::any_cast<void*>(value_)) : value_; }
		};
		struct Typename { };
		struct Variable
		{
			std::string type_, name_;
			std::any value_, init_value_;
		};
		struct Operator
		{
			std::string name;

			Size priority_ul{ 0 };    //unary left
			Size priority_ur{ 0 };    //unary right
			Size priority_b{ 0 };     //binary

			std::map<std::string, std::map<std::string, std::pair<std::string, BinaryOperatorFunction>, std::less<>>, std::less<>> b_funs_;
			std::map<std::string, std::pair<std::string, UnaryOperatorFunction>, std::less<>> ul_funs_;
			std::map<std::string, std::pair<std::string, UnaryOperatorFunction>, std::less<>> ur_funs_;
		};
		struct Function
		{
			std::string name;
			std::vector<std::tuple<std::vector<std::string>, std::string, Calculator::BuiltInFunction>> funs_;
		};
		class Token{
		public:
			enum Type
			{
				NO,               // invalid
				COMMA,            // ,
				SEMICOLON,        // ;
				PARENTHESIS_L,    // (
				PARENTHESIS_R,    // )
				BRACKET_L,        // [
				BRACKET_R,        // ]
				BRACE_L,          // {
				BRACE_R,          // }
				OPERATOR,         // operator
				NUMBER,           // number
				STRING,           // string
				VARIABLE,         // variable
				FUNCTION          // function
			};

			Type type;
			std::string_view word;

			double num;
			const Typename *tpn_;
			const Variable *var;
			const Function *fun;
			const Operator *opr;
		};

		using TokenVec = std::vector<Token>;
		using Iterator = TokenVec::iterator;
		static auto getWord(std::string_view &input)->std::string_view
		{
			static const std::string seperateStr("()[]{},;");
			static const std::string operatorStr("*+-/\\^|<>=&!");
			static const std::string spaceStr(" \t\n\r\f\v");
			static const std::string numStr("0123456789.");
			static const std::string varStr("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789.");

			// triml and check if empty //
			auto next_pos = input.find_first_not_of(spaceStr);
			input = next_pos == std::string_view::npos ? std::string_view() : std::string_view(input.data() + next_pos, input.size() - next_pos);
			if (next_pos == std::string_view::npos)return std::string_view();

			// get number //
			if (input[0] <= '9' && input[0] >= '0'){
				bool has_scientific = false;
				int i;
				for (i = 1; i < input.size(); ++i)
				{
					// 可能是科学计数法 //
					if (i < input.size() - 1
						&& (input[i] == 'e' || input[i] == 'E')
						&& ((input[i + 1] <= '9' && input[i + 1] >= '0') || input[i + 1] == '+' || input[i + 1] == '-'))
					{
						++i;
						continue;
					}

					if (numStr.find(input[i]) == std::string_view::npos) break;
				}

				auto ret = input.substr(0, i);
				input = i == input.size() ? std::string_view() : input.substr(i);
				return ret;
			}
			// get seperator //
			else if (seperateStr.find(input[0]) != seperateStr.npos){
				auto ret = input.substr(0, 1);
				input = 1 == input.size() ? std::string_view() : input.substr(1);
				return ret;
			}
			// get operator //
			else if (operatorStr.find(input[0]) != operatorStr.npos){
				int i;
				for (i = 1; i < input.size() && operatorStr.find(input[i]) != std::string_view::npos; ++i);

				auto ret = input.substr(0, i);
				input = i == input.size() ? std::string_view() : input.substr(i);
				return ret;
			}
			// get string //
			else if (input[0] == '\"'){
				int i;
				for (i = 1; i < input.size() && !(input[i] == '\"' && input[i - 1] != '\\'); ++i);

				auto ret = input.substr(0, i + 1);
				input = i + 1 == input.size() ? std::string_view() : input.substr(i + 1);
				return ret;
			}
			// get word //
			else{
				int i;
				for (i = 1; i < input.size() && varStr.find(input[i]) != std::string_view::npos; ++i);

				auto ret = input.substr(0, i);
				input = i == input.size() ? std::string_view() : input.substr(i);
				return ret;
			}
		}
		auto Expression2Tokens(std::string_view expression)const->TokenVec;
		auto CaculateTokens(Iterator begin_token, Iterator max_end_token) const->Value;

		auto CaculateValueInParentheses(Iterator &begin_token, Iterator max_end_token) const->Value;
		auto CaculateValueInBraces(Iterator &begin_token, Iterator max_end_token) const->Value;
		auto CaculateValueInFunction(Iterator &begin_token, Iterator max_end_token) const->Value;
		auto CaculateValueInOperator(Iterator &begin_token, Iterator max_end_token) const->Value;

		auto FindNextOutsideToken(Iterator left_par, Iterator end_token, Token::Type type) const->Iterator;
		auto FindNextEqualLessPrecedenceBinaryOpr(Iterator begin_token, Iterator end_token, Size precedence)const->Iterator;
		auto GetValues(Iterator begin_token, Iterator end_token)const->std::vector<std::vector<Value> >;

		std::map<std::string, Typename, std::less<>> typename_map_;
		std::map<std::string, Operator, std::less<>> operator_map_;
		std::map<std::string, Function, std::less<>> function_map_;
		std::map<std::string, Variable, std::less<>> variable_map_;

		std::string num_tpn_{ "Number" };
		std::string str_tpn_{ "String" };

		std::function<std::any(double)> num_construct_{ [](double num)->std::any {return num; } };
		std::function<std::any(std::string)> str_construct_{ [](std::string str)->std::any {return str; } };
	};
	auto Calculator::Imp::Expression2Tokens(std::string_view expression)const -> TokenVec
	{
		TokenVec tokens;
		Token token;

		for (token.word = getWord(expression); !token.word.empty(); token.word = getWord(expression))
		{
			switch (*token.word.data())
			{
			case ',':token.type = Token::COMMA; break;
			case ';':token.type = Token::SEMICOLON; break;
			case '(':token.type = Token::PARENTHESIS_L; break;
			case ')':token.type = Token::PARENTHESIS_R; break;
			case '[':token.type = Token::BRACKET_L; break;
			case ']':token.type = Token::BRACKET_R; break;
			case '{':token.type = Token::BRACE_L; break;
			case '}':token.type = Token::BRACE_R; break;
			
			default:
				// 字符串
				if (token.word.data()[0] == '\"'){
					if (token.word.back() != '\"')THROW_FILE_LINE("invalid string:" + std::string(token.word));
					token.type = Token::STRING;
					break;
				}
				// 数字
				if (std::stringstream(std::string(token.word)) >> token.num){
					token.type = Token::NUMBER;
					break;
				}
				// 操作符
				if (operator_map_.find(token.word) != operator_map_.end()){
					token.type = Token::OPERATOR;
					token.opr = &operator_map_.find(token.word)->second;
					break;
				}
				// 变量
				if (variable_map_.find(token.word) != variable_map_.end()){
					token.type = Token::VARIABLE;
					token.var = &variable_map_.find(token.word)->second;
					break;
				}
				// 函数
				if (function_map_.find(token.word) != function_map_.end()){
					token.type = Token::FUNCTION;
					token.fun = &function_map_.find(token.word)->second;
					break;
				}

				THROW_FILE_LINE("unrecognized symbol \"" + std::string(token.word) + "\"");
			}
			tokens.push_back(token);
		}

		return tokens;
	}
	auto Calculator::Imp::CaculateTokens(Iterator b_tok, Iterator e_tok) const ->Value
	{
		if (b_tok >= e_tok)THROW_FILE_LINE("invalid expression");

		Value value;
		for (auto i = b_tok; i < e_tok; )
		{
			// 是否刚开始 //
			if (i == b_tok)
			{
				switch (i->type)
				{
				case Token::PARENTHESIS_L:
					value = CaculateValueInParentheses(i, e_tok);
					break;
				case Token::BRACE_L:
					value = CaculateValueInBraces(i, e_tok);
					break;
				case Token::NUMBER:
					value = Value{ num_tpn_, num_construct_(i->num) };
					i++;
					break;
				case Token::STRING:
					value = Value{ str_tpn_, str_construct_(std::string(i->word.substr(1, i->word.size() - 2))) };
					i++;
					break;
				case Token::OPERATOR:
					value = CaculateValueInOperator(i, e_tok);
					break;
				case Token::VARIABLE:
					value = Value{ i->var->type_, (void*)&i->var->value_ };
					i++;
					break;
				case Token::FUNCTION:
					value = CaculateValueInFunction(i, e_tok);
					break;
				default:
					THROW_FILE_LINE("expression not valid");
				}
			}
			else//如果有当前值,但没有操作符
			{
				if (i->type == Token::OPERATOR)
				{
					if (i->opr->priority_ur)
					{
						auto func = i->opr->ur_funs_.at(value.type_);
						value = Value{ func.first, func.second(value.val()) };
						i++;
					}
					else if (i->opr->priority_b > 0)
					{
						auto e = FindNextEqualLessPrecedenceBinaryOpr(i + 1, e_tok, i->opr->priority_b);
						auto right_value = CaculateTokens(i + 1, e);
						auto func = i->opr->b_funs_.at(value.type_).at(right_value.type_);
						value = Value{ func.first, func.second(value.val(), right_value.val()) };
						i = e;
					}
					else
					{
						THROW_FILE_LINE("expression not valid");
					}
				}
				else
				{
					THROW_FILE_LINE("expression not valid: lack operator");
				}
			}
		}

		return value;
	}
	auto Calculator::Imp::CaculateValueInParentheses(Iterator &i, Iterator max_end_token)const->Value
	{
		auto b_par = i + 1;
		auto e_par = FindNextOutsideToken(i + 1, max_end_token, Token::PARENTHESIS_R);
		i = e_par + 1;

		return CaculateTokens(b_par, e_par);
	}
	auto Calculator::Imp::CaculateValueInBraces(Iterator &i, Iterator max_end_token)const->Value
	{
		auto b_bra = i + 1;
		auto e_bra = FindNextOutsideToken(i + 1, max_end_token, Token::BRACE_R);
		i = e_bra + 1;

		auto values = GetValues(b_bra, e_bra);
		std::vector<std::vector<Matrix>> mtx;
		mtx.reserve(values.size());

		for (auto &r_vec : values)
		{
			mtx.push_back(std::vector<Matrix>(r_vec.size()));
			for (auto &value : r_vec)
			{
				if (value.type_ == "Number")
				{
					mtx.back().push_back(Matrix(std::any_cast<double>(value.val())));
				}
				else if (value.type_ == "Matrix")
				{
					mtx.back().push_back(std::any_cast<Matrix&>(value.val()));
				}
				else
				{
					THROW_FILE_LINE("invalid type when build Matrix");
				}
			}
		}
		auto m = combineMatrices(mtx);
		return Value{ "Matrix", m };
	}
	auto Calculator::Imp::CaculateValueInFunction(Iterator &i, Iterator max_end_token)const->Value
	{
		auto b_par = i + 1;
		if (i + 1 >= max_end_token) THROW_FILE_LINE("invalid expression");
		if (b_par->type != Token::PARENTHESIS_L)THROW_FILE_LINE("function must be followed by \"(\"");

		auto e_par = FindNextOutsideToken(b_par + 1, max_end_token, Token::PARENTHESIS_R);

		// get values, but values dimensions must be 1 x n //
		auto value_mat = GetValues(b_par + 1, e_par);
		if (value_mat.size() != 1)THROW_FILE_LINE("function \"" + std::string(i->word) + "\" + do not has invalid param type");

		// transfer values to param types and param values //
		auto params = value_mat.front();
		std::vector<std::string> p_types(params.size());
		std::vector<std::any> p_values(params.size());
		for (int i = 0; i < params.size(); ++i)
		{
			p_types[i] = params[i].type_;
			p_values[i] = params[i].val();
		}

		// search functions //
		auto &funs = function_map_.find(i->word)->second.funs_;
		if (auto f = std::find_if(funs.begin(), funs.end(), [&](const auto &v)->bool {return p_types.size() == std::get<0>(v).size() && std::equal(p_types.begin(), p_types.end(), std::get<0>(v).begin()); }); f == funs.end())
		{
			THROW_FILE_LINE("function \"" + std::string(i->word) + "\" not found");
		}
		else
		{
			i = e_par + 1;
			return Value{ std::get<1>(*f), std::get<2>(*f)(p_values) };
		}
	}
	auto Calculator::Imp::CaculateValueInOperator(Iterator &i, Iterator max_end_token)const->Value
	{
		auto opr = i;
		i = FindNextEqualLessPrecedenceBinaryOpr(opr + 1, max_end_token, opr->opr->priority_ul);
		auto value = CaculateTokens(opr + 1, i);
		auto func = opr->opr->ul_funs_.at(value.type_);
		return Value{func.first, func.second(value.val())};
	}
	auto Calculator::Imp::FindNextOutsideToken(Iterator begin_token, Iterator end_token, Token::Type type)const->Iterator
	{
		Size par_num = 0, bra_num = 0, bce_num = 0;
		auto next_token = begin_token;

		for (;next_token < end_token; ++next_token)
		{
			if ((par_num == 0) && (bra_num == 0) && (bce_num == 0) && (next_token->type == type))return next_token;
			switch (next_token->type)
			{
			case Token::PARENTHESIS_L:par_num++; break;
			case Token::PARENTHESIS_R:par_num--; break;
			case Token::BRACKET_L:bra_num++; break;
			case Token::BRACKET_R:bra_num--; break;
			case Token::BRACE_L:bce_num++; break;
			case Token::BRACE_R:bce_num--; break;
			default:break;
			}
		}

		return end_token;
	}
	auto Calculator::Imp::FindNextEqualLessPrecedenceBinaryOpr(Iterator begin_token, Iterator end_token, Size precedence)const->Iterator
	{
		auto next_opr = begin_token;
		for (; next_opr < end_token; ++next_opr)
		{
			next_opr = FindNextOutsideToken(next_opr, end_token, Token::OPERATOR);
			if ((next_opr == end_token) || (next_opr->opr->priority_b <= precedence))break;
		}

		return next_opr;
	}
	auto Calculator::Imp::GetValues(Iterator b_tok, Iterator e_tok)const->std::vector<std::vector<Value> >
	{
		std::vector<std::vector<Value> > ret;

		auto b_row = b_tok;
		while (b_row < e_tok)
		{
			auto e_row = FindNextOutsideToken(b_row, e_tok, Token::SEMICOLON);
			auto b_col = b_row;

			ret.push_back(std::vector<Value>());

			while (b_col < e_row)
			{
				auto e_col = FindNextOutsideToken(b_col, e_row, Token::COMMA);
				ret.back().push_back(CaculateTokens(b_col, e_col));
				b_col = e_col == e_tok ? e_col : e_col + 1;
			}

			b_row = e_row == e_tok ? e_row : e_row + 1;
		}

		return ret;
	}
	auto Calculator::calculateExpression(std::string_view expression) const->std::pair<std::string, std::any>
	{
		auto tokens = imp_->Expression2Tokens(expression);
		auto ret_val = imp_->CaculateTokens(tokens.begin(), tokens.end());
		return std::make_pair(std::move(ret_val.type_), std::move(ret_val.val()));
	}
	auto Calculator::setNumberType(std::string_view tpn, std::function<std::any(double)> construct)->void
	{
		if (imp_->typename_map_.find(tpn) == imp_->typename_map_.end()) THROW_FILE_LINE("\"" + std::string(tpn) + "not exists, can't set number with this type");
		
		imp_->num_tpn_ = tpn;
		imp_->num_construct_ = construct;
	}
	auto Calculator::setStringType(std::string_view tpn, std::function<std::any(std::string)> construct)->void
	{
		if (imp_->typename_map_.find(tpn) == imp_->typename_map_.end()) THROW_FILE_LINE("\"" + std::string(tpn) + "not exists, can't set string with this type");

		imp_->str_tpn_ = tpn;
		imp_->str_construct_ = construct;
	}
	auto Calculator::addTypename(std::string_view tpn)->void
	{
		if (imp_->typename_map_.find(tpn) != imp_->typename_map_.end() ||
			imp_->variable_map_.find(tpn) != imp_->variable_map_.end() ||
			imp_->function_map_.find(tpn) != imp_->function_map_.end())
		{
			THROW_FILE_LINE("\"" + std::string(tpn) + "already exists, can't add this type");
		}

		imp_->typename_map_.insert(std::pair(std::string(tpn), Imp::Typename()));

		addFunction(tpn, std::vector<std::string>{ std::string(tpn) }, tpn, [](std::vector<std::any>& v)->std::any {return v[0]; });
		addBinaryOperatorFunction("=", tpn, tpn, tpn, [](std::any& p1, std::any&p2)->std::any {	return p1 = p2;	});
	}
	auto Calculator::addVariable(std::string_view var, std::string_view type, const std::any &value)->void
	{
		if (imp_->typename_map_.find(var) != imp_->typename_map_.end() ||
			imp_->variable_map_.find(var) != imp_->variable_map_.end() ||
			imp_->function_map_.find(var) != imp_->function_map_.end())
		{
			THROW_FILE_LINE("\"" + std::string(var) + "already exists, can't add this variable");
		}

		imp_->variable_map_.insert(std::pair(std::string(var), Imp::Variable{std::string(type), std::string(var), value, value}));
	}
	auto Calculator::addFunction(std::string_view fun, const std::vector<std::string> &params, std::string_view ret_type, BuiltInFunction f)->void
	{
		if (imp_->variable_map_.find(fun) != imp_->variable_map_.end())
		{
			THROW_FILE_LINE("\"" + std::string(fun) + "already exists, can't add this function");
		}
		
		auto &funs = imp_->function_map_[std::string(fun)].funs_;
		if (std::find_if(funs.begin(), funs.end(), [&](const auto &val)->bool {	return std::equal(params.begin(), params.end(), std::get<0>(val).begin()); }) != funs.end())
			THROW_FILE_LINE("function \"" + std::string(fun) + "already exists, can't add function");

		funs.push_back(std::make_tuple(params, std::string(ret_type), f));
	}
	auto Calculator::addOperator(std::string_view opr, int ul_priority, int ur_priority, int b_priority)->void
	{
		if (imp_->operator_map_.find(opr) != imp_->operator_map_.end())THROW_FILE_LINE("Already has operator:" + std::string(opr));

		imp_->operator_map_[std::string(opr)].priority_ul = ul_priority;
		imp_->operator_map_[std::string(opr)].priority_ur = ur_priority;
		imp_->operator_map_[std::string(opr)].priority_b = b_priority;
	}
	auto Calculator::addUnaryLeftOperatorFunction(std::string_view opr, std::string_view p_type, std::string_view ret_type, UnaryOperatorFunction f)->void
	{
		if (auto found_opr = imp_->operator_map_.find(opr);found_opr == imp_->operator_map_.end())THROW_FILE_LINE("failed add operator func:" + std::string(opr));
		else if(auto found_type = imp_->typename_map_.find(p_type); found_type == imp_->typename_map_.end())THROW_FILE_LINE("failed add operator func:" + std::string(p_type));
		else if (auto found_func = found_opr->second.ul_funs_.find(p_type); found_func != found_opr->second.ul_funs_.end())THROW_FILE_LINE("failed add operator func:" + std::string(opr));
		else
		{
			found_opr->second.ul_funs_.insert(std::make_pair(std::string(p_type), std::make_pair(std::string(ret_type), f)));
		}
	}
	auto Calculator::addUnaryRightOperatorFunction(std::string_view opr, std::string_view p_type, std::string_view ret_type, UnaryOperatorFunction f)->void
	{
		if (auto found_opr = imp_->operator_map_.find(opr); found_opr == imp_->operator_map_.end())THROW_FILE_LINE("failed add operator func:" + std::string(opr));
		else if (auto found_type = imp_->typename_map_.find(p_type); found_type == imp_->typename_map_.end())THROW_FILE_LINE("failed add operator func:" + std::string(p_type));
		else if (auto found_func = found_opr->second.ur_funs_.find(p_type); found_func != found_opr->second.ur_funs_.end())THROW_FILE_LINE("failed add operator func:" + std::string(opr));
		else
		{
			found_opr->second.ur_funs_.insert(std::make_pair(std::string(p_type), std::make_pair(std::string(ret_type), f)));
		}
	}
	auto Calculator::addBinaryOperatorFunction(std::string_view opr, std::string_view p1_type, std::string_view p2_type, std::string_view ret_type, BinaryOperatorFunction f)->void
	{
		if (auto found_opr = imp_->operator_map_.find(opr); found_opr == imp_->operator_map_.end())THROW_FILE_LINE("failed add operator func:" + std::string(opr));
		else if (auto found_type1 = imp_->typename_map_.find(p1_type); found_type1 == imp_->typename_map_.end())THROW_FILE_LINE("failed add operator func:" + std::string(p1_type));
		else if (auto found_type2 = imp_->typename_map_.find(p2_type); found_type2 == imp_->typename_map_.end())THROW_FILE_LINE("failed add operator func:" + std::string(p2_type));
		else if (auto found = found_opr->second.b_funs_.find(p1_type); found != found_opr->second.b_funs_.end() && found->second.find(p2_type) != found->second.end())
			THROW_FILE_LINE("failed add operator func:" + std::string(opr));
		else
		{
			if (found == found_opr->second.b_funs_.end())
				found_opr->second.b_funs_.insert(std::make_pair(std::string(p1_type), std::map<std::string, std::pair<std::string, BinaryOperatorFunction>, std::less<>>()));

			found_opr->second.b_funs_[std::string(p1_type)][std::string(p2_type)] = std::make_pair(std::string(ret_type), f);
		}
	}
	auto Calculator::clearVariables()->void { imp_->variable_map_.clear(); }
	auto Calculator::clearAllRules()->void 
	{
		imp_->function_map_.clear();
		imp_->operator_map_.clear();
		imp_->typename_map_.clear();
		imp_->variable_map_.clear();
	}
	auto Calculator::resetVariablesToInitValue()->void {
		for (auto &v : imp_->variable_map_)v.second.value_ = v.second.init_value_;
	}
	Calculator::~Calculator() = default;
	Calculator::Calculator(const std::string &name)
	{
		addOperator("=", 0, 0, 1);
		
		addTypename("String");
		addTypename("Number");
		addTypename("Matrix");

		addOperator("+", 10, 0, 10);
		addUnaryLeftOperatorFunction("+", "Number", "Number", [](std::any& p1)->std::any {return std::any_cast<double>(p1); });
		addBinaryOperatorFunction("+", "Number", "Number", "Number", [](std::any& p1, std::any&p2)->std::any
		{
			return std::any_cast<double>(p1) + std::any_cast<double>(p2);
		});
		addBinaryOperatorFunction("+", "Number", "Matrix", "Matrix", [](std::any& p1, std::any&p2)->std::any
		{
			return std::any_cast<double>(p1) + std::any_cast<aris::core::Matrix&>(p2);
		});
		addBinaryOperatorFunction("+", "Matrix", "Number", "Matrix", [](std::any& p1, std::any&p2)->std::any
		{
			return std::any_cast<aris::core::Matrix&>(p1) + std::any_cast<double>(p2);
		});
		addBinaryOperatorFunction("+", "Matrix", "Matrix", "Matrix", [](std::any& p1, std::any&p2)->std::any
		{
			return std::any_cast<aris::core::Matrix&>(p1) + std::any_cast<aris::core::Matrix>(p2);
		});

		addOperator("-", 10, 0, 10);
		addUnaryLeftOperatorFunction("-", "Number", "Number", [](std::any& p1)->std::any {return -std::any_cast<double>(p1); });
		addBinaryOperatorFunction("-", "Number", "Number", "Number", [](std::any& p1, std::any&p2)->std::any
		{
			return std::any_cast<double>(p1) - std::any_cast<double>(p2);
		});
		addBinaryOperatorFunction("-", "Number", "Matrix", "Matrix", [](std::any& p1, std::any&p2)->std::any
		{
			return std::any_cast<double>(p1) - std::any_cast<aris::core::Matrix&>(p2);
		});
		addBinaryOperatorFunction("-", "Matrix", "Number", "Matrix", [](std::any& p1, std::any&p2)->std::any
		{
			return std::any_cast<aris::core::Matrix&>(p1) - std::any_cast<double>(p2);
		});
		addBinaryOperatorFunction("-", "Matrix", "Matrix", "Matrix", [](std::any& p1, std::any&p2)->std::any
		{
			return std::any_cast<aris::core::Matrix&>(p1) - std::any_cast<aris::core::Matrix>(p2);
		});

		addOperator("*", 0, 0, 20);
		addBinaryOperatorFunction("*", "Number", "Number", "Number", [](std::any& p1, std::any&p2)->std::any
		{
			return std::any_cast<double>(p1) * std::any_cast<double>(p2);
		});
		addBinaryOperatorFunction("*", "Number", "Matrix", "Matrix", [](std::any& p1, std::any&p2)->std::any
		{
			return std::any_cast<double>(p1) * std::any_cast<aris::core::Matrix&>(p2);
		});
		addBinaryOperatorFunction("*", "Matrix", "Number", "Matrix", [](std::any& p1, std::any&p2)->std::any
		{
			return std::any_cast<aris::core::Matrix&>(p1) * std::any_cast<double>(p2);
		});
		addBinaryOperatorFunction("*", "Matrix", "Matrix", "Matrix", [](std::any& p1, std::any&p2)->std::any
		{
			return std::any_cast<aris::core::Matrix&>(p1) * std::any_cast<aris::core::Matrix>(p2);
		});

		addOperator("/", 0, 0, 20);
		addBinaryOperatorFunction("/", "Number", "Number", "Number", [](std::any& p1, std::any&p2)->std::any
		{
			return std::any_cast<double>(p1) / std::any_cast<double>(p2);
		});

		addOperator("++", 100, 100, 0);
		addUnaryLeftOperatorFunction("++", "Number", "Number", [](std::any& p1)->std::any {	return std::any_cast<double&>(p1) += 1.0; });
		addUnaryRightOperatorFunction("++", "Number", "Number", [](std::any& p1)->std::any
		{
			auto ret = std::any_cast<double>(p1);
			std::any_cast<double&>(p1) += 1.0;
			return ret;
		});

		addOperator("--", 100, 100, 0);
		addUnaryLeftOperatorFunction("--", "Number", "Number", [](std::any& p1)->std::any {	return std::any_cast<double&>(p1) -= 1.0; });
		addUnaryRightOperatorFunction("--", "Number", "Number", [](std::any& p1)->std::any
		{
			auto ret = std::any_cast<double>(p1);
			std::any_cast<double&>(p1) -= 1.0;
			return ret;
		});

#define ADD_MATRIX_OPR(OPR) \
		[](std::any& p1, std::any&p2)->std::any \
		{\
			auto &m1 = std::any_cast<aris::core::Matrix&>(p1);\
			auto &m2 = std::any_cast<aris::core::Matrix&>(p2);\
			Matrix ret;\
			if (m1.size() == 1)\
			{					\
				ret = Matrix(m2);\
				for (auto &d : ret)d = m1(0, 0) OPR d;\
			}\
			else if (m2.size() == 1)\
			{\
				ret = Matrix(m1);\
				for (auto &d : ret) d = d OPR m2(0, 0);\
			}\
			else if ((m1.m() == m2.m()) && (m1.n() == m2.n()))\
			{\
				ret = Matrix(m1);\
									\
				for (Size i = 0; i < ret.size(); ++i)\
				{\
					ret.data()[i] = m1.data()[i] OPR m2.data()[i];\
				}\
			}\
			else\
			{\
				THROW_FILE_LINE("dimensions are not equal");\
			}\
			return ret;\
		}

#define ADD_NUM_MATRIX_OPR(OPR) \
		[](std::any& p1, std::any&p2)->std::any \
		{\
			auto &m1 = std::any_cast<double&>(p1);\
			auto &m2 = std::any_cast<aris::core::Matrix&>(p2);\
			Matrix ret = m2;\
			for (Size i = 0; i < ret.size(); ++i)\
			{\
				ret.data()[i] = m1 OPR m2.data()[i];\
			}\
			return ret;\
		}

#define ADD_MATRIX_NUM_OPR(OPR) \
		[](std::any& p1, std::any&p2)->std::any \
		{\
			auto &m1 = std::any_cast<aris::core::Matrix&>(p1);\
			auto &m2 = std::any_cast<double&>(p2);\
			Matrix ret = m1;\
			for (Size i = 0; i < ret.size(); ++i)\
			{\
				ret.data()[i] = m1.data()[i] OPR m2;\
			}\
			return ret;\
		}

#define ADD_NUMBER_OPR(OPR) \
		[](std::any& p1, std::any&p2)->std::any \
		{\
			return double(std::any_cast<double&>(p1) OPR std::any_cast<double&>(p2)); \
		}

		addOperator("<", 1, 0, 1);
		addBinaryOperatorFunction("<", "Matrix", "Matrix", "Matrix", ADD_MATRIX_OPR(<));
		addBinaryOperatorFunction("<", "Number", "Matrix", "Matrix", ADD_NUM_MATRIX_OPR(<));
		addBinaryOperatorFunction("<", "Matrix", "Number", "Matrix", ADD_MATRIX_NUM_OPR(<));
		addBinaryOperatorFunction("<", "Number", "Number", "Number", ADD_NUMBER_OPR(<));

		addOperator("<=", 1, 0, 1);
		addBinaryOperatorFunction("<=", "Matrix", "Matrix", "Matrix", ADD_MATRIX_OPR(<=));
		addBinaryOperatorFunction("<=", "Number", "Matrix", "Matrix", ADD_NUM_MATRIX_OPR(<=));
		addBinaryOperatorFunction("<=", "Matrix", "Number", "Matrix", ADD_MATRIX_NUM_OPR(<=));
		addBinaryOperatorFunction("<=", "Number", "Number", "Number", ADD_NUMBER_OPR(<=));

		addOperator(">", 1, 0, 1);
		addBinaryOperatorFunction(">", "Matrix", "Matrix", "Matrix", ADD_MATRIX_OPR(> ));
		addBinaryOperatorFunction(">", "Number", "Matrix", "Matrix", ADD_NUM_MATRIX_OPR(> ));
		addBinaryOperatorFunction(">", "Matrix", "Number", "Matrix", ADD_MATRIX_NUM_OPR(> ));
		addBinaryOperatorFunction(">", "Number", "Number", "Number", ADD_NUMBER_OPR(> ));

		addOperator(">=", 1, 0, 1);
		addBinaryOperatorFunction(">=", "Matrix", "Matrix", "Matrix", ADD_MATRIX_OPR(>= ));
		addBinaryOperatorFunction(">=", "Number", "Matrix", "Matrix", ADD_NUM_MATRIX_OPR(>= ));
		addBinaryOperatorFunction(">=", "Matrix", "Number", "Matrix", ADD_MATRIX_NUM_OPR(>= ));
		addBinaryOperatorFunction(">=", "Number", "Number", "Number", ADD_NUMBER_OPR(>= ));

		addOperator("==", 1, 0, 1);
		addBinaryOperatorFunction("==", "Matrix", "Matrix", "Matrix", ADD_MATRIX_OPR(== ));
		addBinaryOperatorFunction("==", "Number", "Matrix", "Matrix", ADD_NUM_MATRIX_OPR(== ));
		addBinaryOperatorFunction("==", "Matrix", "Number", "Matrix", ADD_MATRIX_NUM_OPR(== ));
		addBinaryOperatorFunction("==", "Number", "Number", "Number", ADD_NUMBER_OPR(== ));

		addVariable("pi", "Number", double(3.141592653));
		addFunction("sin", std::vector<std::string>{"Number"}, "Number", [](std::vector<std::any> &params)->std::any {
			return std::sin(std::any_cast<double>(params[0]));
		});
	}
	ARIS_DEFINE_BIG_FOUR_CPP(Calculator);
	
	struct LanguageParser::Imp{
		struct CmdInfo{
			std::string cmd;
			int next_cmd_true_, next_cmd_false_;
		};

		struct FuncParamData { std::string type_, name_, value_; };

		struct FuncData {
			std::string file_;
			int line_;
			std::vector<FuncParamData> params_;
		};
		struct FuncStackData {
			std::string file_;
			int line_;
			std::vector<FuncParamData> params_;
		};
		
		

		// 关键词 //
		static inline const char *MAIN         = "main";
		static inline const char *ENDMAIN      = "endmain";
		static inline const char *FUNCTION     = "function";
		static inline const char *ENDFUNCTION  = "endfunction";
		static inline const char *VAR          = "var";
		static inline const char *IF           = "if";
		static inline const char *ELSE         = "else";
		static inline const char *ELSEIF       = "elseif";
		static inline const char *ENDIF        = "endif";
		static inline const char *WHILE        = "while";
		static inline const char *ENDWHILE     = "endwhile";

		using Iterator = std::map<int, CmdInfo>::iterator;
		auto parseEnvironment(std::string file_name, Iterator b, Iterator e)->Iterator{
			for (auto i = b; i != e; ++i){
				auto id = i->first;
				auto &info = i->second;
				auto cmd_name = info.cmd.substr(0, info.cmd.find_first_of(" \t\n\r\f\v("));

				if (auto l = { ENDMAIN, ENDWHILE, IF, ELSEIF, ELSE, ENDIF, WHILE, ENDWHILE };
				std::any_of(l.begin(), l.end(), [&cmd_name](const char *c) {return c == cmd_name; })){
					auto err = "invalid " + info.cmd + " in line:" + std::to_string(id);
					THROW_FILE_LINE(err);
				}
				else if (cmd_name == VAR){
					variables_.push_back(info.cmd);
				}
				else if (cmd_name == MAIN){
					if (main_line_)THROW_FILE_LINE("program has more than 1 main function");
					main_line_ = i->first;
					main_file_ = file_name;
					if (std::next(i) != e)i->second.next_cmd_true_ = std::next(i)->first;
					i = parseMain(std::next(i), e);
				}
				else if (cmd_name == FUNCTION){
					if (info.cmd.find_first_of(" \t\n\r\f\v(") == std::string::npos)	THROW_FILE_LINE("function does not have name in line: " + std::to_string(i->first));
					// func string //
					auto func_body = info.cmd.substr(info.cmd.find_first_of(" \t\n\r\f\v("), std::string::npos);
					func_body.erase(0, func_body.find_first_not_of(" \t\n\r\f\v"));
					func_body.erase(func_body.find_last_not_of(" \t\n\r\f\v") + 1);
					// func name //
					auto func_name = func_body.substr(0, func_body.find_first_of(" \t\n\r\f\v("));
					if (functions_.find(func_name) != functions_.end()){
						THROW_FILE_LINE("function already exist in line " + std::to_string(i->first));
					}
					// func params //
					FuncData func_data{ file_name, i->first,{} };

					auto func_param = func_body.substr(func_body.find_first_of(" \t\n\r\f\v("), std::string::npos);
					if (func_param.size() < 2 || func_param.front()!='(' || func_param.back()!=')')
						THROW_FILE_LINE("function format not correct in line " + std::to_string(i->first));

					// 获取参数 //
					auto params = func_param.substr(1, func_param.size() - 2);
					auto find_param_type_and_name = [&](std::size_t start, std::size_t end) {
						static const std::string param_str("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_0123456789"); // 和之前的var name相比少一个.

						auto type_and_name = params.substr(start, end - start);
						type_and_name.erase(0, type_and_name.find_first_not_of(" \t\n\r\f\v"));
						type_and_name.erase(type_and_name.find_last_not_of(" \t\n\r\f\v") + 1);

						if (type_and_name.find_first_of(" \t\n\r\f\v") == std::string::npos)
							THROW_FILE_LINE("function format not correct in line " + std::to_string(i->first));

						auto type = type_and_name.substr(0, type_and_name.find_first_of(" \t\n\r\f\v"));
						auto name = type_and_name.substr(type_and_name.find_first_of(" \t\n\r\f\v"), std::string::npos);

						name.erase(0, name.find_first_not_of(" \t\n\r\f\v"));
						name.erase(name.find_last_not_of(" \t\n\r\f\v") + 1);

						func_data.params_.push_back(FuncParamData{ type, name });
					};
					std::size_t start = 0U, end = params.find(",");
					for (; 
						end != std::string::npos; 
						start = end + 1, end = params.find(",", start)) 
					{
						find_param_type_and_name(start, end);
					}
					if (start < params.size())find_param_type_and_name(start, params.size());



					// insert //
					functions_.insert(std::make_pair(func_name, func_data));

					if (std::next(i) != e)i->second.next_cmd_true_ = std::next(i)->first;
					i = parseFunction(std::next(i), e);
				}
				else
				{
					auto err = "invalid " + info.cmd + " in line:" + std::to_string(id);
					THROW_FILE_LINE(err);
				}
			}

			return e;
		}
		auto parseMain(Iterator b, Iterator e)->Iterator{
			for (auto i = b; i != e; ++i){
				auto id = i->first;
				auto &info = i->second;
				auto cmd_name = info.cmd.substr(0, info.cmd.find_first_of(" \t\n\r\f\v("));

				if (cmd_name == ENDMAIN){
					auto ret = parseCode(b, i);
					std::prev(i)->second.next_cmd_true_ = i->first;
					end_main_line_ = id;
					return ret;
				}
			}

			std::string err = "no endmain for main function in line " + std::to_string(b->first);
			THROW_FILE_LINE(err);

			return e;
		}
		auto parseFunction(Iterator b, Iterator e)->Iterator{
			for (auto i = b; i != e; ++i){
				auto id = i->first;
				auto &info = i->second;
				auto cmd_name = info.cmd.substr(0, info.cmd.find_first_of(" \t\n\r\f\v("));

				if (cmd_name == ENDFUNCTION){
					auto ret = parseCode(b, i);
					std::prev(i)->second.next_cmd_true_ = i->first;
					return ret;
				}
			}

			std::string err = "no endfuncion for function in line " + std::to_string(b->first);
			THROW_FILE_LINE(err);

			return e;
		}
		auto parseCode(Iterator b, Iterator e)->Iterator{
			for (auto i = b; i != e; ++i){
				auto id = i->first;
				auto &info = i->second;
				auto cmd_name = info.cmd.substr(0, info.cmd.find_first_of(" \t\n\r\f\v("));

				if (auto l = { MAIN, ENDMAIN, FUNCTION, ENDFUNCTION, VAR, ELSEIF, ELSE, ENDIF, ENDWHILE };
					std::any_of(l.begin(), l.end(), [&cmd_name](const char *c) {return c == cmd_name; }))
				{
					auto err = "invalid " + info.cmd + " in line:" + std::to_string(id);
					THROW_FILE_LINE(err);
				}
				else if (cmd_name == IF){
					if (std::next(i) != e)i->second.next_cmd_true_ = std::next(i)->first;
					i = parseIf(i, e);
				}
				else if (cmd_name == WHILE){
					if (std::next(i) != e)i->second.next_cmd_true_ = std::next(i)->first;
					i = parseWhile(i, e);
				}
				else{
					info.next_cmd_true_ = std::next(i) == e ? 0 : std::next(i)->first;
				}
			}

			return e;
		}
		auto parseIf(Iterator b, Iterator e)->Iterator{
			std::list<Iterator> prev_else_line;
			Iterator last_if_begin = b;

			int is_end = 1;
			bool has_else = false;
			for (auto i = std::next(b); i != e; ++i){
				auto id = i->first;
				auto &info = i->second;
				auto cmd_name = info.cmd.substr(0, info.cmd.find_first_of(" \t\n\r\f\v("));

				if (cmd_name == IF){
					is_end++;
				}
				else if (cmd_name == ELSEIF && is_end == 1){
					if (has_else){
						std::string err = "Find elseif after else in line " + std::to_string(i->first);
						THROW_FILE_LINE(err);
					}

					parseCode(std::next(last_if_begin), i);
					last_if_begin->second.next_cmd_false_ = i->first;
					last_if_begin = i;
					i->second.next_cmd_true_ = std::next(i)->first;

					prev_else_line.push_back(std::prev(i));
				}
				else if (cmd_name == ELSE && is_end == 1)
				{
					if (has_else)
					{
						std::string err = "Find second else in line " + std::to_string(i->first);
						THROW_FILE_LINE(err);
					}
					has_else = true;

					parseCode(std::next(last_if_begin), i);
					last_if_begin->second.next_cmd_false_ = i->first;
					last_if_begin = i;
					i->second.next_cmd_true_ = std::next(i)->first;

					prev_else_line.push_back(std::prev(i));
				}
				else if (cmd_name == ENDIF)
				{
					is_end--;

					if (is_end == 0)
					{
						parseCode(std::next(last_if_begin), i);
						auto prev_cmd_name = std::prev(i)->second.cmd.substr(0, std::prev(i)->second.cmd.find_first_of(" \t\n\r\f\v("));
						if (prev_cmd_name != "if") std::prev(i)->second.next_cmd_true_ = i->first;
						i->second.next_cmd_true_ = std::next(i) == e ? 0 : std::next(i)->first;
						auto last_if_begin_cmd_name = last_if_begin->second.cmd.substr(0, last_if_begin->second.cmd.find_first_of(" \t\n\r\f\v("));
						if (last_if_begin_cmd_name != "else")last_if_begin->second.next_cmd_false_ = i->first;

						for (auto j : prev_else_line)
						{
							j->second.next_cmd_true_ = i->first;
						}

						return i;
					}
				}
			}

			std::string err = "no endif for if in line " + std::to_string(b->first);
			THROW_FILE_LINE(err);

			return b;
		}
		auto parseWhile(Iterator b, Iterator e)->Iterator
		{
			int is_end = 1;
			for (auto i = std::next(b); i != e; ++i)
			{
				auto id = i->first;
				auto &info = i->second;
				auto cmd_name = info.cmd.substr(0, info.cmd.find_first_of(" \t\n\r\f\v("));

				if (cmd_name == WHILE)
				{
					is_end++;
				}
				else if (cmd_name == ENDWHILE)
				{
					is_end--;

					if (is_end == 0)
					{
						parseCode(std::next(b), i);
						std::prev(i)->second.next_cmd_true_ = b->first;
						i->second.next_cmd_true_ = std::next(i) == e ? 0 : std::next(i)->first;
						b->second.next_cmd_false_ = i->first;

						return i;
					}
				}
			}

			return b;
		}

		// 原始文件 //
		std::map<std::string, std::string> files_;

		// 编译结果 //
		std::map<std::string, std::map<int, CmdInfo>>  cmds_;
		std::map<std::string, FuncData> functions_;                      //  名字    所在文件  行号
		std::vector<std::string> variables_;                             //  所在文件  变量定义(定义的时候才取名)
		std::string main_file_;
		int main_line_, end_main_line_;

		// 运行时 //
		std::list<FuncStackData> func_ret_stack_;
		
		std::string current_file_;
		int current_line_;
	};
	auto LanguageParser::parseLanguage()->void 
	{ 
		for (auto &file : imp_->files_){
			imp_->parseEnvironment(file.first, imp_->cmds_[file.first].begin(), imp_->cmds_[file.first].end());
		}

		if (imp_->main_line_ == 0)THROW_FILE_LINE("program must has main");
		this->gotoMain();
	}
	auto LanguageParser::setProgram(std::map<std::string, std::string> program)->void
	{
		// 设置文件 //
		imp_->files_ = program;
		
		// 清理编译期变量 //
		imp_->cmds_.clear();
		imp_->functions_.clear();
		imp_->variables_.clear();
		imp_->main_file_ = "";
		imp_->main_line_ = 0;
		
		for (auto &file : imp_->files_)
		{
			std::stringstream ss(file.second);
			imp_->cmds_[file.first];

			int id;
			while (ss >> id)
			{
				char c;
				while (ss >> c, c != ':');
				std::string cmd;
				std::getline(ss, cmd);
				cmd.erase(0, cmd.find_first_not_of(" \t\n\r\f\v"));// trim l
				cmd.erase(cmd.find_last_not_of(" \t\n\r\f\v") + 1);// trim r
				if (cmd != "")imp_->cmds_[file.first][id] = Imp::CmdInfo{ cmd, 0, 0 };
			}
		}
	}
	auto LanguageParser::hasCursor()->bool {
		return imp_->cmds_.find(imp_->current_file_) != imp_->cmds_.end()
			&& imp_->cmds_[imp_->current_file_].find(imp_->current_line_) != imp_->cmds_[imp_->current_file_].end();
	}
	auto LanguageParser::varPool()->const std::vector<std::string>& { return imp_->variables_; }
	auto LanguageParser::gotoMain()->void
	{
		imp_->current_file_ = imp_->main_file_;
		imp_->current_line_ = imp_->main_line_;
		imp_->func_ret_stack_.clear();
	}
	auto LanguageParser::gotoFileLine(std::string file, int line)->void
	{
		if (imp_->cmds_.find(file) == imp_->cmds_.end())THROW_FILE_LINE("This file does not exist");
		auto file_cmds = imp_->cmds_[file];
		if (file_cmds.find(line) == file_cmds.end())THROW_FILE_LINE("This line does not exist");

		imp_->current_file_ = file;
		imp_->current_line_ = line;
		imp_->func_ret_stack_.clear();
	}
	auto LanguageParser::forward(bool is_this_cmd_successful)->void{
		auto cmd_str = currentCmd().substr(0, currentCmd().find_first_of(" \t\n\r\f\v("));

		if (auto found = imp_->functions_.find(cmd_str); found != imp_->functions_.end()){
			Imp::FuncStackData stack_data{ imp_->current_file_, imp_->cmds_[imp_->current_file_][imp_->current_line_].next_cmd_true_ ,found->second.params_ };
			
			// 获取函数的参数，并传入到函数中 //
			std::string param_str(currentParamStr());
			if (param_str.size() < 2 || param_str.front() != '(' || param_str.back() != ')')
				THROW_FILE_LINE("function format not correct in line " + std::to_string(currentLine()));

			param_str = param_str.substr(1, param_str.size() - 2);

			int id = 0;
			auto find_param_name = [&](std::size_t start, std::size_t end) {
				auto param = param_str.substr(start, end - start);
				param.erase(0, param.find_first_not_of(" \t\n\r\f\v"));
				param.erase(param.find_last_not_of(" \t\n\r\f\v") + 1);

				stack_data.params_[id].value_ = param;
				id++;
			};

			std::size_t start = 0U, end = param_str.find(",");
			for (;
				end != std::string::npos;
				start = end + 1, end = param_str.find(",", start))
			{
				find_param_name(start, end);
			}
			if (start < param_str.size())find_param_name(start, param_str.size());

			imp_->func_ret_stack_.push_back(stack_data);
			imp_->current_file_ = found->second.file_;
			imp_->current_line_ = found->second.line_;
		}
		else if (cmd_str == Imp::ENDFUNCTION){
			if (imp_->func_ret_stack_.empty()){
				// 什么都不做
			}
			else{
				imp_->current_file_ = imp_->func_ret_stack_.back().file_;
				imp_->current_line_ = imp_->func_ret_stack_.back().line_;
				imp_->func_ret_stack_.pop_back();
			}
		}
		else{
			imp_->current_line_ = is_this_cmd_successful ? imp_->cmds_[imp_->current_file_][imp_->current_line_].next_cmd_true_ : imp_->cmds_[imp_->current_file_][imp_->current_line_].next_cmd_false_;
		}
	}
	auto LanguageParser::currentLine()const->int { return imp_->current_line_; }
	auto LanguageParser::currentFile()const->std::string { return imp_->current_file_; }
	auto LanguageParser::currentCmd()const->std::string { 
		std::string line = imp_->cmds_.at(imp_->current_file_).at(imp_->current_line_).cmd;
		
		if (line.substr(0, line.find_first_of(" \t\n\r\f\v([{}])")) != Imp::FUNCTION && !imp_->func_ret_stack_.empty()) {
			line = " "+line;	// c++11 regex doesn't support look behind
			for (auto &param : imp_->func_ret_stack_.back().params_){
				std::regex pattern("([^\\.])\\b" + param.name_ + "\\b(?!\\.)");
				line = std::regex_replace(line, pattern, "$1"+param.value_);
			}
			line = line.substr(1);
		}
		
		return line;
	}
	auto LanguageParser::currentWord()const->std::string
	{
		std::string v = currentCmd();
		return v.substr(0, v.find_first_of(" \t\n\r\f\v([{}])"));
	}
	auto LanguageParser::currentParamStr()const->std::string
	{
		std::string v = currentCmd();
		return v.substr(v.find_first_of(" \t\n\r\f\v([{}])"));
	}
	auto LanguageParser::isCurrentLineKeyWord()const->bool
	{
		auto cmd_name = currentCmd().substr(0, currentCmd().find_first_of(" \t\n\r\f\v("));
		auto l = { Imp::MAIN, Imp::ENDMAIN, Imp::FUNCTION, Imp::ENDFUNCTION	, Imp::VAR, Imp::IF, Imp::ELSE, Imp::ELSEIF, Imp::ENDIF, Imp::WHILE, Imp::ENDWHILE, };
		return std::any_of(l.begin(), l.end(), [&cmd_name](const char *c) {return c == cmd_name; });
	}
	auto LanguageParser::isCurrentLineFunction()const->bool
	{
		auto cmd_name = currentCmd().substr(0, currentCmd().find_first_of(" \t\n\r\f\v("));
		return imp_->functions_.find(cmd_name) != imp_->functions_.end();
	}
	auto LanguageParser::isEnd()const->bool
	{
		return (currentCmd() == Imp::ENDFUNCTION && imp_->func_ret_stack_.empty()) || currentCmd() == Imp::ENDMAIN;
	}
	LanguageParser::~LanguageParser() = default;
	LanguageParser::LanguageParser(const std::string &name) :imp_(new Imp) {}
	ARIS_DEFINE_BIG_FOUR_CPP(LanguageParser);

	ARIS_REGISTRATION{
		aris::core::class_<aris::core::Matrix>("Matrix")
			.textMethod(
				[](Matrix *v)->std::string{
					return v->toString();
				}, 
				[](Matrix *v, std::string_view str)->void{
					static aris::core::Calculator c;
					static int i = 0;
					if (i == 0)	{
						c.addVariable("PI", "Number", double(aris::PI));
						i = 1;
					}
			
					auto mat = c.calculateExpression(std::string("Matrix({") + std::string(str) + "})").second;
					*v = std::any_cast<const Matrix&>(mat);
				});
	}
}