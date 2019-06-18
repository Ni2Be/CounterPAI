#include "Piano_Player.h"
#include "Midi_Reader.h"

int main()
{
	Midi_Reader midi_reader("D:/Programmieren/TH/CounterPAI/CounterPAI/CounterPAI/data/piano.mid", 0);

	Piano_Player player;

	player.play_sheet_music(midi_reader.sheet);


	char ch = 'n';
	int i = 0;
	while (ch != 'y')
	{
		player.update();
	}
	std::cin >> ch;
}

//
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
//	SVG_Parser bass_clef("data/pictures/icons/clef-bass.svg", "data/pictures/icons/clef-bass.png", 20);
//	sf::Sprite sprite;
//	sprite.setTexture(bass_clef.m_texture);
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
//
//			if (event.type == sf::Event::MouseButtonPressed)
//			{
//				sprite_pos = sf::Vector2f(sprite.getPosition().x - sf::Mouse::getPosition(window).x, sprite.getPosition().y - sf::Mouse::getPosition(window).y);
//				mouse_is_down = true;
//				keyboard.alto_play(44);
//
//			}
//			if (event.type == sf::Event::MouseButtonReleased)
//			{
//				mouse_is_down = false; 
//				keyboard.soprano_play(46);
//
//			}
//			if (mouse_is_down)
//			{
//				std::cout << sf::Mouse::getPosition(window).x << " " << sf::Mouse::getPosition(window).y  << "\n";
//				sprite.setPosition(sprite_pos.x + sf::Mouse::getPosition(window).x, sprite_pos.y + sf::Mouse::getPosition(window).y);
//			}
//
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


//
////LibTorch Test
//
//#include "Net.h"
//
//
//int main()
//{
//	int in_size = 3, out_size = 1;
//
//	auto model = std::make_shared<Net>(Net(in_size, out_size));
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
//	auto learn_data = torch::randn({ in_size });
//	auto target_data = torch::randn({ out_size });
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
//
