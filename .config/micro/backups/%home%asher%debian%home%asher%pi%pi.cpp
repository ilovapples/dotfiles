#include <gmpxx.h>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

mpz_class fact(int n) {
	mpz_t ret; mpz_init_set_ui(ret, 1);
	for (int i=2; i<=n; ++i) {
		mpz_mul_ui(ret, ret, i);
	}
	return mpz_class(ret);
}

int main(int argc, char* argv[]) {
	mpf_set_default_prec(64 * (300/20));
	//mpf_class pi_35("3.14159265358979323846264338327950288");

	mpf_t pi_get; mpf_init_set_ui(pi_get,0);

	int appr_max_digits = 30000;
	mpf_set_prec(pi_get,64 * (appr_max_digits/20));
	
	unsigned long int iter = 30UL;

	if (argc > 1) {
		iter = std::stoi(argv[1]);
	}

	std::cout << "Calculating pi with " << iter << " iterations of the Chudnovsky algorithm.";
	
	for (unsigned long int k=0UL; k<iter; k++) {
		short int sign_i = pow(-1,k); // "zeroth" term: (-1)^k

		mpz_class term1 = fact(6*k); // first term: (6k)!
		mpf_t term1f_t; mpf_init(term1f_t); mpf_set_z(term1f_t, term1.get_mpz_t()); 
		
		mpz_t temp_0,temp_1,temp_2;
		mpz_init_set_ui(temp_0,13591409);
		mpz_init_set_ui(temp_1,k);
		mpz_init_set_ui(temp_2,545140134);
		mpz_addmul(temp_0,temp_1,temp_2);
		mpf_t term2f_t; mpf_init(term2f_t); mpf_set_z(term2f_t, temp_0);
		// second term (multiplied by zeroth and first): (545140134k+13591409)

		mpz_class term3 = fact(3*k); // third term (in the denominator, multiplied by the rest of the terms): (3k)!
		mpf_t term3f_t; mpf_init(term3f_t); mpf_set_z(term3f_t, term3.get_mpz_t());
		

		mpz_t temp_3; mpz_init(temp_3); 
		mpz_pow_ui(temp_3,fact(k).get_mpz_t(),3);
		mpf_t term4f_t; mpf_init(term4f_t); mpf_set_z(term4f_t, temp_3);
		// fourth term: (k!)^3

		// 640320^(3k+3/2) = 640320^(3k) * sqrt(640320^3)
		mpz_t temp_40; mpz_init_set_ui(temp_40, 640320);
		mpz_t temp_41; mpz_init(temp_41);
		mpz_pow_ui(temp_41,temp_40,3*k); // 640320^(3k)
		mpz_t temp_50; mpz_init(temp_50);
		mpz_pow_ui(temp_50,temp_40,3);
		mpf_t temp_5; mpf_init(temp_5); mpf_set_z(temp_5,temp_50);
		mpf_t temp_42; mpf_init(temp_42); mpf_set_z(temp_42,temp_41); // convert temp_41 to mpf_t to be multiplied later
		mpf_t temp_6; mpf_init(temp_6); 
		mpf_set_prec(temp_6,64*(appr_max_digits)/20);
		mpf_sqrt(temp_6,temp_5); // sqrt(640320^3)
		mpf_t term5; mpf_init(term5); 
		mpf_set_prec(term5,64*(appr_max_digits)/20);
		mpf_mul(term5, temp_42, temp_6); // fifth term: 640320^(3k+(3/2))

		mpf_t numerator; mpf_init_set_si(numerator,sign_i);
		mpf_mul(numerator, numerator, term1f_t);
		mpf_mul(numerator, numerator, term2f_t);

		mpf_t denominator; mpf_init_set_ui(denominator,1);
		mpf_set_prec(denominator,64*(appr_max_digits)/20);
		mpf_mul(denominator, denominator, term3f_t);
		mpf_mul(denominator, denominator, term4f_t);
		mpf_mul(denominator, denominator, term5);

		mpf_t el; mpf_init(el);
		mpf_set_prec(el,64*(appr_max_digits)/20);
		mpf_div(el, numerator, denominator);

		mpf_add(pi_get, pi_get, el);

		mpf_clear(term1f_t); mpf_clear(term2f_t); mpf_clear(term3f_t); mpf_clear(term4f_t); mpf_clear(temp_5);
		mpf_clear(temp_42);  mpf_clear(temp_6);   mpf_clear(numerator);mpf_clear(denominator);mpf_clear(el);
		mpz_clear(temp_0);   mpz_clear(temp_1);   mpz_clear(temp_2);   mpz_clear(temp_3);   mpz_clear(temp_40);
		mpz_clear(temp_41);  mpz_clear(temp_50);

		std::cout << "Iteration " << k << " complete!\n";
	}
	mpf_mul_ui(pi_get, pi_get, 12);
	mpf_t num_1;mpf_init_set_ui(num_1,1); //literally the number 1
	mpf_div(pi_get, num_1, pi_get);
	mpf_class pi(pi_get);

	mp_exp_t exp;
	std::string significand = pi.get_str(exp);
	

	// get contents of check.txt as a string
	std::ifstream file("check.txt");
	if (!file.is_open()) {
		std::cout << "no check.txt present";
		return 0;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string check = buffer.str();
	
	int correct = 0;
	for (int i=1; i<(int)check.size(); i++) {
		if (check[i] != ' ') {
			if (check[i] == significand[i]) {
				correct++;
			} else {
				std::ofstream output("log.txt");
				
				//print 
				std::cout << "Significand:\n3";
				if (output.is_open()) { output << "Significand:\n3"; }
				for (int i0=1;i0<(int)significand.size(); i0++) {
					if (correct == i0) {
						std::cout << significand[i0] << "  ";
						if (output.is_open()) { output << significand[i0] << "  "; }
					} else {
						std::cout << significand[i0];
						if (output.is_open()) { output << significand[i0]; }
					}
				}

				std::cout << "\nExp: " << exp-(int)significand.size() << std::endl;
				std::cout << "Correct to " << correct << " decimal place" << ((correct!=1) ? "s" : "") << ".\n";

				if (output.is_open()) {
					output << "\nExp: " << exp-(int)significand.size() << std::endl; 
					output << "Correct to " << correct << " decimal place" << ((correct!=1) ? "s" : "") << ".\n";
					output << "Correct places:\n3";
					for (int i0=1;i0<(int)significand.size(); i0++) {
						if (correct == i0) {
							output << significand[i0] << "  ";
							break;
						} else {
							output << significand[i0];
						}
					}
					output.close(); 
				}

				return 0;
			}
		}
	}
	
	return 0;
}
