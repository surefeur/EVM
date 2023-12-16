#include <iostream>
#include <vector>

using namespace std;

struct Parameters {
	float P1, P2;
	size_t N, M;
	Parameters() {
		P1 = 0.6;
		N = 10;
		M = 16;
		P2 = 0.5;
	}
};

static float rand_float() {
	return (float)(rand()) / (float)(RAND_MAX);
}

struct Command {
private:
	Parameters parameters;
	size_t current_step;
	float P_second_oper;
	float P_result;

	size_t cnt_N, cnt_M;
	vector<size_t> cnt_tic;

	bool finished;
	bool waiting;

public:
	Command() {
		P_second_oper = 0.0;
		P_result = 0.0;

		current_step = 1;
		cnt_N = 0;
		cnt_M = 0;
		
		cnt_tic.resize(5);

		finished = false;
		waiting = false;
	}
	
	vector<size_t> get_cnt_tic() {
		return cnt_tic;
	}
	/*
	* За один такт -> 1 дешифрация и 2 выполения команд
	*				  1 - 1    &&    2 - (2 - 5)
	*/
	bool isFinished() const {
		return finished;
	}

	void in_waiting() {
		waiting = true;
	}

	void out_of_waiting() {
		waiting = false;
	}

	void choice_step() {
		if (finished) return;
		if (waiting) {
			cnt_tic[current_step - 1]++;
			return;
		}

		switch (current_step) {
		case 1: // чтение и дешифрация кода операции ----> 1 такт
			deshifr();
			break;
		case 2: // первый операнд
			first_oper();
			break;
		case 3: // второй операнд
			// вычислить вероятность события здесь
			// double P = rand() % 1;
			// P <= P1 -> регистр -> +1 tic
			// P > P1 -> адрес -> +N tic 
			P_second_oper = rand_float();
			second_oper(P_second_oper);
			break;
		case 4: // вычисление результата
			// вычислить вероятность события здесь
			// double P = rand() % 1;
			// P <= P2 -> команда первого типа -> +1 tic
			// P > P2 -> команда второго типа -> +M tic
			P_result = rand_float();
			result(P_result);
			break;
		case 5: // запись данных (по адресу второго операнда
			write_to_memory();
			break;

		default:
			break;
		}
	}

	void deshifr() {
		// +1 tic
		cnt_tic[current_step - 1]++;
		current_step++;
	}

	void first_oper() {
		// находится в регистре
		// +1 tic
		cnt_tic[current_step - 1]++;
		current_step++;
	}

	void second_oper(float P) {
		// регистр +1 tic
		// адрес +N tic
		if (cnt_N == 0 && P <= parameters.P1) {
			// +1 tic
			cnt_tic[current_step - 1]++;
			
			current_step++;
		}
		else {
			// +1 tic
			cnt_tic[current_step - 1]++;
			
			cnt_N++;
			if (cnt_N == parameters.N) {
				current_step++;
			}
		}
	}

	void result(float P) {
		// команда первого типа -> +1 tic
		// команда второго типа -> +M tic
		if (cnt_M == 0 && P <= parameters.P2) {
			// +1 tic
			cnt_tic[current_step - 1]++;

			current_step++;
		}
		else {
			// +1 tic
			cnt_tic[current_step - 1]++;

			cnt_M++;
			if (cnt_M == parameters.M) {
				current_step++;
			}
		}
	}

	void write_to_memory() {
		// +1 tic
		cnt_tic[current_step - 1]++;

		finished = true;
	}
};


struct Conveyer {
private:
	vector<Command> v_c;
	vector<size_t> indx_perf;
	size_t cnt_tic;
	size_t cnt_command;
	size_t current_cnt_command;
public:
	Conveyer() {
		cnt_command = 100;
		current_cnt_command = 0;
		cnt_tic = 0;
		v_c.resize(cnt_command);
	}

	void start_conveyr() {
		while (v_c.back().isFinished() == false) {
			if (current_cnt_command < cnt_command)
				current_cnt_command++;
		
			for (size_t i = 0; i < current_cnt_command; i++) {
				if (v_c[i].isFinished() == false) {
					if (indx_perf.size() < 2) {
						indx_perf.push_back(i);
						v_c[i].out_of_waiting();
					}
					else {
						v_c[i].in_waiting();
					}
				}
			}

			for (size_t i = 0; i < current_cnt_command; i++) {
				if (v_c[i].isFinished()) continue;

				v_c[i].choice_step();
			}

			// +1 tic
			indx_perf.clear();
		}
		vector<float> avrg_tic(5);
		for (int i = 0; i < cnt_command; i++) {
			for (int j = 0; j < 5; j++) {
				avrg_tic[j] += v_c[i].get_cnt_tic()[j];
			}
		}
		for (int i = 0; i < 5; i++) {
			avrg_tic[i] /= cnt_command;
		}

		for (int i = 0; i < 5; i++) {
			cout << i + 1 << "step :" << avrg_tic[i] << '\n';
		}
	}
};

int main() {
	srand(time(0));
	Conveyer cnv;
	cnv.start_conveyr();
} 