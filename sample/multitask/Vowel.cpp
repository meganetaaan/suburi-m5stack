#include <string>
#include <memory>
#include <complex>
#include <vector>
#include <cmath>
#include <sndfile.h>
#include <fftw3.h>
// #include <boost/format.hpp>

using std::complex;
using std::vector;
using std::pair;

// LPC 係数の次数
const int LPC_ORDER = 64;

// 解析に用いる微小区間 (sec)
const double WINDOW_DURATION = 0.04;

// FFT する
vector<double> fft(const vector<double>& input);

// LPC スペクトル包絡を得る
vector<double> lpc(const vector<double>& input, int order, double df);

// [1 : -1] に正規化
vector<double> normalize(const vector<double>& input);

// ハミング窓をかける
vector<double> hamming(const vector<double>& input);

// デジタルフィルタ
vector<double> freqz(const vector<double>& b, const vector<double>& a, double df, int N);

// フォルマント f1 / f2 を返す
pair<double, double> formant(const vector<double>& input, double df);

// フォルマントから母音を推定する
std::string vowel(double f1, double f2);

// 与えられた範囲の音の平均
double volume(const vector<double>& input);


int main()
{
	// 音声ファイル読み込み
	SF_INFO sinfo;
	std::shared_ptr<SNDFILE> sf(
		sf_open("aiueo.wav", SFM_READ, &sinfo), [](SNDFILE* ptr) { sf_close(ptr); });
	const int frame = sinfo.frames;
	std::unique_ptr<short[]> input(new short[frame]);
	const double sample_rate = sinfo.samplerate;
	const double dt = 1.0 / sample_rate;
	sf_read_short(sf.get(), input.get(), frame);

	// 微小音声区間取り出し
	const int window_size = static_cast<int>(WINDOW_DURATION / dt);
	const double df  = 1.0 / (window_size / sample_rate);
	for (int n = 0; n < frame / window_size; ++n) {
		vector<double> data;
		for (int i = window_size * n; i < window_size * (n + 1) && i < frame; ++i) {
			data.push_back(input[i]);
		}
		auto hamming_result = normalize( hamming(data) );

		auto lpc_result = normalize( lpc(hamming_result, LPC_ORDER, df) );
		auto fft_result = normalize( fft(hamming_result) );

		// 得られた LPC スペクトル包絡線からフォルマントを抽出
		auto formant_result = formant(lpc_result, df);
		double f1 = formant_result.first;
		double f2 = formant_result.second;

		if (volume(data) < 1e4) {
			std::cout << "-";
		} else {
			std::cout << boost::format("%1%  (f1:%2%, f2:%3%)")
				% vowel(f1, f2) % f1 % f2;
		}
		std::cout << std::endl;
	}

	// Gnuplot で結果を確認
	std::shared_ptr<FILE> freq_graph(
		popen("gnuplot -persist", "w"), [](FILE* ptr) { pclose(ptr); });
	fprintf(freq_graph.get(), "load 'freq.plt'\n");

	// std::shared_ptr<FILE> wav_graph(
	// 	popen("gnuplot -persist", "w"), [](FILE* ptr) { pclose(ptr); });
	// fprintf(wav_graph.get(), "load 'wav.plt'\n");

	return 0;
}


// FFT
vector<double> fft(const vector<double>& input)
{
	const int frame = input.size();
	vector<complex<double>> in(frame), out(frame);
	auto plan = fftw_plan_dft_1d(
			frame,
			reinterpret_cast<fftw_complex*>(&in[0]),
			reinterpret_cast<fftw_complex*>(&out[0]),
			FFTW_FORWARD, FFTW_ESTIMATE
	);
	for (int i = 0; i < frame; ++i) {
		in[i] = complex<double>(input[i], 0.0);
	}
	fftw_execute(plan);

	vector<double> fft_result(frame);
	for (int i = 0; i < frame; ++i) {
		fft_result[i] = abs(out[i]);
	}

	return fft_result;
}


// LPC
vector<double> lpc(const vector<double>& input, int order, double df)
{
	const int N = input.size();

	// 自己相関関数
	vector<double> r(N);
	const int lags_num = order + 1;
	for (int l = 0; l < lags_num; ++l) {
		r[l] = 0.0;
		for (int n = 0; n < N - l; ++n) {
			r[l] += input[n] * input[n + l];
		}
	}

	// Levinson-Durbin のアルゴリズムで LPC 係数を計算
	vector<double> a(order + 1, 0.0), e(order + 1, 0.0);
	a[0] = e[0] = 1.0;
	a[1] = - r[1] / r[0];
	e[1] = r[0] + r[1] * a[1];
	for (int k = 1; k < order; ++k) {
		double lambda = 0.0;
		for (int j = 0; j < k + 1; ++j) {
			lambda -= a[j] * r[k + 1 - j];
		}
		lambda /= e[k];

		vector<double> U(k + 2), V(k + 2);
		U[0] = 1.0; V[0] = 0.0;
		for (int i = 1; i < k + 1; ++i) {
			U[i] = a[i];
			V[k + 1 - i] = a[i];
		}
		U[k + 1] = 0.0; V[k + 1] = 1.0;

		for (int i = 0; i < k + 2; ++i) {
			a[i] = U[i] + lambda * V[i];
		}

		e[k + 1] = e[k] * (1.0 - lambda * lambda);
	}

	// LPC 係数から音声信号再現
	// vector<double> lpc_result(N, 0.0);
	// for (int i = 0; i < N; ++i) {
	// 	if (i < order) {
	// 		lpc_result[i] = input[i];
	// 	} else {
	// 		for (int j = 1; j < order; ++j) {
	// 			lpc_result[i] -= a[j] * input[i + 1 - j];
	// 		}
	// 	}
	// }
	// return lpc_result;

	return freqz(e, a, df, N);
}


// 正規化
vector<double> normalize(const vector<double>& input)
{
	// 最大 / 最小値
	auto max = abs( *std::max_element(input.begin(), input.end()) );
	auto min = abs( *std::min_element(input.begin(), input.end()) );
	double factor = std::max(max, min);
	vector<double> result( input.size() );
	std::transform(input.begin(), input.end(), result.begin(), [factor](double x) {
		return x / factor;
	});
	return result;
}


// ハミング窓をかける
vector<double> hamming(const vector<double>& input)
{
	const double N = input.size();
	vector<double> result(N);
	for (int i = 1; i < N - 1; ++i) {
		const double h = 0.54 - 0.46 * cos(2 * M_PI * i / (N - 1));
		result[i] = input[i] * h;
	}
	result[0] = result[N - 1] = 0;
	return result;
}


// デジタルフィルタ
vector<double> freqz(const vector<double>& b, const vector<double>& a, double df, int N)
{
	vector<double> H(N);
	for (int n = 0; n < N + 1; ++n) {
		auto z = std::exp(complex<double>(0.0, -2.0 * M_PI * n / N));
		complex<double> numerator(0.0, 0.0), denominator(0.0, 0.0);
		for (int i = 0; i < b.size(); ++i) {
			numerator += b[b.size() - 1 - i] * pow(z, i);
		}
		for (int i = 0; i < a.size(); ++i) {
			denominator += a[a.size() - 1 - i] * pow(z, i);
		}
		H[n] = abs(numerator / denominator);
	}

	return H;
}


// フォルマント f1 / f2 を返す
pair<double, double> formant(const vector<double>& input, double df)
{
	pair<double, double> result(0.0, 0.0);
	bool is_find_first = false;
	for (int i = 1; i < input.size() - 1; ++i) {
		if (input[i] > input[i-1] && input[i] > input[i+1]) {
			if (!is_find_first) {
				result.first = df * i;
				is_find_first = true;
			} else {
				result.second = df * i;
				break;
			}
		}
	}
	return result;
}


// 母音を推定
// NOTE: 機械学習にする予定
std::string vowel(double f1, double f2)
{
	if (f1 > 600 && f1 < 1400 && f2 > 900  && f2 < 2000) return "a";
	if (f1 > 100 && f1 < 410  && f2 > 1900 && f2 < 3500) return "i";
	if (f1 > 100 && f1 < 700  && f2 > 1100 && f2 < 2000) return "u";
	if (f1 > 400 && f1 < 800  && f2 > 1700 && f2 < 3000) return "e";
	if (f1 > 300 && f1 < 900  && f2 > 500  && f2 < 1300) return "o";
	return "-";
}


// ボリューム
double volume(const vector<double>& input)
{
	double v = 0.0;
	std::for_each(input.begin(), input.end(), [&v](double x) { v += x*x; });
	v /= input.size();
	return v;
}
