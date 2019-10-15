#define RAPIDJSON_HAS_STDSTRING 1


#include "Application.h"
#include <SFML/Graphics.hpp>

#include "Utility.h"


int main(int argc, char** argv)
{
	bool start_ai_cli = false;
	if (argc > 1)
		start_ai_cli = argv[1];

	Application app(start_ai_cli);
	return app.run();
}

////CUDATEST
//#include <torch/torch.h>
//
//struct Net : torch::nn::Module
//{
//	Net(int64_t input, int64_t hidden, int64_t output, int64_t hidden_layer_count, torch::Device device);
//	torch::Device device;
//	torch::nn::LSTM lstm{ nullptr };
//	torch::nn::Linear out{ nullptr };
//	float s_learning_rate = 0.001;
//	torch::Tensor forward(torch::Tensor input);
//	torch::Tensor learn_step(torch::Tensor learn_data, torch::Tensor target_data, bool optimize = true);
//	std::shared_ptr<torch::optim::Optimizer> optimizer;
//};
//Net::Net(int64_t input, int64_t hidden, int64_t output, int64_t hidden_layer_count, torch::Device device)
//	:
//	device(device),
//	lstm(torch::nn::LSTMOptions(input, hidden).layers(hidden_layer_count))
//{
//	out = register_module("out", torch::nn::Linear(hidden, output));
//	register_module("lstm", lstm);
//	optimizer = std::make_shared<torch::optim::Adam>(torch::optim::Adam(this->parameters(), s_learning_rate));
//	this->to(device);
//}
//
//torch::Tensor Net::forward(torch::Tensor x)
//{
//	torch::nn::RNNOutput lstm_out = lstm->forward(x);
//	x = lstm_out.output;
//	x = out->forward(x).sigmoid();
//	return x;
//}
//
//torch::Tensor Net::learn_step(torch::Tensor learn_data, torch::Tensor target_data, bool optimize)
//{
//	this->zero_grad();
//	auto new_prediction = this->forward(learn_data);
//	torch::Tensor loss;
//	loss = torch::mse_loss(new_prediction, target_data).to(device);
//	if (optimize)
//	{
//		loss.backward();
//		optimizer->step();
//	}
//	return loss;
//}
//
//int main()
//{
//	try
//	{
//		int in_size = 3, hidden_size = 10, hidden_layer_count = 2, out_size = 1;
//		int sequence_len = 10, batch_size = 400;
//		torch::Device device = torch::kCUDA;
//		//torch::Device device = torch::kCPU;
//		auto model = std::make_shared<Net>(Net(in_size, hidden_size, out_size, hidden_layer_count, device));
//
//		auto feature_data = torch::randn({ sequence_len, batch_size, in_size }).to(device);
//		auto target_data = torch::randn({ sequence_len, batch_size, out_size }).to(device);
//
//		std::cout << std::endl << "Prediction..." << std::endl;
//		auto prediction = model->forward(feature_data);
//
//		std::cout << std::endl << "learning..." << std::endl;
//		for (size_t epoch = 0; epoch < 500; epoch++)
//		{
//			auto loss = model->learn_step(feature_data, target_data);
//			if (epoch % 100 == 0)
//				std::cout << "epoch: " << epoch << ", Loss: " << loss << std::endl;
//		}
//		std::cout << std::endl << "End!" << std::endl;
//	}
//	catch (std::exception& e)
//	{
//		std::cerr << e.what();
//	}
//	char ch;
//	std::cin >> ch;
//}

//#include "Piano_Player.h"
//#include "Midi_Reader.h"
//
//int main()
//{
//	Midi_Reader midi_reader("data/piano.mid", 0);
//
//	Piano_Player player;
//
//	player.play_sheet_music(midi_reader.sheet);
//
//
//	char ch = 'n';
//	int i = 0;
//	while (ch != 'y')
//	{
//		player.update();
//	}
//	std::cin >> ch;
//}


////SFML Test / nanosvg Test / Sound
////
//#include "SVG_Parser.h"
//#include <iostream>
//#include <SFML/Graphics.hpp>
//#include "Keyboard.h"
//
//int main()
//{
//	//Keyboard
//	Keyboard keyboard;
//
//	//WINDOW DRAG SVG LOAD
//	sf::RenderWindow window(sf::VideoMode(1200, 480), "CounterPAI");
//
//	sf::Sprite sprite;
//	sf::Texture texture;
//	for (int i = 0; i < 1; i++)
//	{
//		SVG_Parser bass_clef("data/pictures/icons/clef-bass.svg", 20);
//		texture = bass_clef.m_texture;
//		sprite.setTexture(texture);
//	}
//	sf::Vector2f sprite_pos;
//	bool mouse_is_down = false;
//	while (window.isOpen())
//	{
//		sf::Event event;
//		
//		while (window.pollEvent(event))
//		{
//			if (event.type == sf::Event::Closed)
//				window.close();
//		}
//
//		window.clear(sf::Color::White);
//		window.draw(sprite);
//		window.display();
//	}
//
//	return 0;
//}

////Midifile Test
//
//#include <MidiFile.h>
//#include <Options.h>
//#include "Keyboard.h"
//
//
//#include <vector>
//#include <iostream>
//#include <iomanip>
//
//using namespace std;
//using namespace smf;
//
//int main(int argc, char** argv) 
//{
//	//Keyboard
//	Keyboard keyboard;
//	Options options;
//	options.process(argc, argv);
//	MidiFile midifile;
//	if (options.getArgCount() == 0) midifile.read("D:/Programmieren/TH/CounterPAI/CounterPAI/CounterPAI/data/testmidi.mid");
//	else midifile.read(options.getArg(1));
//	midifile.doTimeAnalysis();
//	midifile.linkNotePairs();
//
//	int tracks = midifile.getTrackCount();
//	cout << "TPQ: " << midifile.getTicksPerQuarterNote() << endl;
//	if (tracks > 1) cout << "TRACKS: " << tracks << endl;
//	for (int track = 0; track < tracks; track++) {
//		if (tracks > 1) cout << "\nTrack " << track << endl;
//		cout << "Tick\tSeconds\tDur\tMessage" << endl;
//		for (int event = 0; event < midifile[track].size(); event++) {
//			cout << dec << midifile[track][event].tick;
//			cout << '\t' << dec << midifile[track][event].seconds;
//			cout << '\t';
//			if (midifile[track][event].isNoteOn())
//				cout << midifile[track][event].getDurationInSeconds();
//			cout << '\t';// << hex;
//
//
//			//TEST
//			
//			cout << (int)midifile[track][event][1] << " <----\n";
//
//			if ((int)midifile[track][event][0] == 144)
//			{
//				keyboard.alto_play((int)midifile[track][event][1] + 12);
//				sf::sleep(sf::seconds(midifile[track][event].getDurationInSeconds()));
//			}
//
//			for (int i = 0; i < midifile[track][event].size(); i++)
//				cout << (int)midifile[track][event][i] << ' ';
//			cout << endl;
//		}
//	}
//	char ch;
//	cin >> ch;
//	return 0;
//}



////LibTorch Test
//
//#include <torch/torch.h>
//
//#include "Net.h"
//
//
//int main()
//{
//	using namespace Eval;
//	int in_size = 3, hidden_size = 10, out_size = 1;
//
//	auto model = std::make_shared<Net>(Net(in_size, hidden_size, out_size));
//	model->set_learning_rate(0.4);
//	
//	
//	////Eigener:
//	//std::vector<float> vec = { 3.3,   4.4,   5.5,
//	//							6.71,  6.93,  4.168,
//	//							9.779, 6.182, 7.59 ,
//	//                            3.3,   4.4,   5.5,
//	//							6.71,  6.93,  4.168,
//	//							9.779, 6.182, 7.59 ,
//	//						    3.3,   4.4,   5.5,
//	//							6.71,  6.93,  4.168,
//	//							9.779, 6.182, 7.59 };
//	//
//	//auto eigener = at::tensor(vec,
//	//	at::requires_grad(false).dtype(torch::kFloat32))
//	//	.view({ 27,1 });
//	//std::cout << "eigener:\n" << eigener << std::endl << std::endl;
//	//auto eigener2 = at::tensor(vec,
//	//	at::requires_grad(false).dtype(torch::kFloat32))
//	//	.view({ 3,3,3 });
//	//std::cout << "eigener2:\n" << eigener2 << std::endl << "dim: " << eigener2.dim() << std::endl << "data: " << *(eigener2[2][2][2].data<float>()) << std::endl;
//	//
//	
//
//	auto learn_data = torch::randn({ 1, 1, in_size });
//	auto target_data = torch::randn({ 1, 1, out_size });
//	std::cout << "learn_data :\n" << learn_data << std::endl << std::endl;
//	std::cout << "target_data :\n" << target_data << std::endl << std::endl;
//
//
//	std::cout << std::endl << "Vor lernen: " << std::endl;
//	for (const auto& p : model->named_parameters())
//	{
//		std::cout << p.key() << std::endl << p.value() << std::endl;
//	}
//
//	auto prediction = model->forward(learn_data);
//	std::cout << std::endl << std::endl << "Prediction:\n" << prediction << std::endl;
//
//	std::cout << std::endl << "learning..." << std::endl;
//	for (size_t epoch = 0; epoch < 1000; epoch++)
//	{
//		auto loss = model->learn_step(learn_data, target_data);
//
//		if (epoch % 100 == 0)
//		{
//			std::cout << "epoch: " << epoch << ", Loss: " << loss << std::endl;
//			torch::save(model, "model.pt");
//		}
//	}
//
//
//	std::cout <<  std::endl << "Nach lernen: " << std::endl;
//	for (const auto& p : model->named_parameters())
//	{
//		std::cout << p.key() << std::endl << p.value() << std::endl;
//	}
//
//
//	prediction = model->forward(learn_data);
//	std::cout << std::endl << std::endl << "inputn_data :\n" << learn_data ;
//	std::cout << std::endl << std::endl << "target_data normal:\n" << target_data;
//	std::cout << std::endl << std::endl << "Prediction normal:\n" << prediction << std::endl;
//
//	char ch;
//	std::cin >> ch;
//}

