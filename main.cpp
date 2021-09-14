#include <iostream>				// std::cout
#include <fstream>				// for creating WAV file.
#include <SFML/Graphics.hpp>			// For drawing the wave function.
#include <Windows.h>				// For playing the wave file.
#include <vector>				// std::vector ( for multiple point ).

using namespace std;

// Riff Chunk
const string chunk_id = "RIFF";
const string chunk_size = "...."; // 4 Byte we don't what the size is.
const string format = "WAVE";

// FMT sub-chunk
const string subchunck1_id = "fmt ";
const int subchunck1_size = 16;
const int audio_format = 1;
const int num_channels = 2;
const int sample_rate = 44100;
const int byte_rate = sample_rate * num_channels * (subchunck1_size / 8); // divide by 8 to byte
const int black_align = num_channels * (subchunck1_size / 8);
const int bits_per_sample = 16;

// Data sub-chunck
const string subchunck2_id = "data";
const string subchunck2_size = "....";

const int duration = 2;
const int max_amplitude = 32760;
const double frequency = 678;


void write_as_bytes(ofstream& file, int value, int byte_size)
{
	file.write(reinterpret_cast<const char*> (&value), byte_size);

}

int main()
{
	int newAmplitude = 7; // Change this to get a new wave.
	sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(800, 800), "AUDIO Graph");
	sf::Event ev;
	
	// Circle point.
	sf::CircleShape point;
	vector<sf::CircleShape> points;
	point.setRadius(1);
	point.setFillColor(sf::Color::Red);
	point.setPosition(0, 800);
	points.push_back(point);

	// Audio File Reading
	ofstream wav;
	wav.open("test.wav", ios::binary);

	// Writing WAV file.
	if (wav.is_open())
	{
		wav << chunk_id;
		wav << chunk_size;
		wav << format;

		wav << subchunck1_id;
		write_as_bytes(wav, subchunck1_size, 4);
		write_as_bytes(wav, audio_format, 2);
		write_as_bytes(wav, num_channels, 2);
		write_as_bytes(wav, sample_rate, 4);
		write_as_bytes(wav, byte_rate, 4);
		write_as_bytes(wav, black_align, 2);
		write_as_bytes(wav, bits_per_sample, 2);

		wav << subchunck2_id;
		wav << subchunck2_size;

		int start_audio = wav.tellp(); // tells me where am i.

		for (int i = 0; i < sample_rate; i++)
		{
			// Respect max amplitude
			// Operate as a wave
			//y(t) = A * sin(2 * PI * f * t + shift)
			double amplitude = (double)(i / sample_rate) * max_amplitude ;
			double value = sin((3 * 3.14 * (i)*frequency) / sample_rate) * newAmplitude;

			double channel1 = (amplitude * value);
			double channel2 = (max_amplitude - amplitude) * value;

			write_as_bytes(wav, channel1, 2);
			write_as_bytes(wav, channel2, 2);

			point.setPosition(i, (value)+400);
			points.push_back(point);
		}

		int end_audio = wav.tellp();
		wav.seekp(start_audio - 4);
		write_as_bytes(wav, end_audio - start_audio, 4);


		wav.seekp(4, ios::beg); // 4 bits after the beginning

		write_as_bytes(wav, end_audio - 8, 4);
	}

	wav.close();

	// Play the sound.
	PlaySound(TEXT("test.wav"), NULL, SND_FILENAME);

	// Main Game Loop.
	while (window->isOpen())
	{
		// Pollevents
		while (window->pollEvent(ev)) 
			if (ev.type == sf::Event::Closed) window->close();
	
		// Clear Old Frame
		window->clear();

		// Draw all the points.
		for (auto& pnt : points)
			window->draw(pnt);

		// Render Objects;
		window->display();
	
	}

	points.clear();

	return 0;
}
