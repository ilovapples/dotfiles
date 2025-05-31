use t3online::*;
use std::{
	env, 
	io::{self, Read, Write}, 
	net::{
		// TcpListener, 
		TcpStream},
	process::exit
};
use crossterm::{
	cursor::*, event::*, execute, style::*, terminal::{
		Clear,ClearType,
		enable_raw_mode, disable_raw_mode},
};

include!(concat!(env!("OUT_DIR"), "/env_constants.rs"));

fn cleanup_for_selmove() {
	disable_raw_mode().unwrap();
	execute!(
		io::stdout(),
		Show,
	).unwrap();
}

const BUF_SIZE: usize = 512;
fn update_screen_for_selmove(space: u8, board: T3Board) {
	execute!(
		io::stdout(),
		Clear(ClearType::All),
		MoveTo(0,0),
	).unwrap();
	board.print();
	execute!(io::stdout(),
		MoveTo(4*(space%3) as u16,2*(space as f64/3.0).floor() as u16),
		SetBackgroundColor(Color::White),
		SetForegroundColor(Color::Black),
		
		Print(match space {
			0 => "┌───┬",
			1 => "┬───┬",
			2 => "┬───┐",
			3 | 6 => "├───┼",
			4 | 7 => "┼───┼",
			5 | 8 => "┼───┤",
			_ => "",
		}),
		MoveDown(1),
		MoveLeft(5),
		Print(format!("│ {} │", 
			T3Board::to_str(board.grid[space as usize]),
		)),
		MoveDown(1),
		MoveLeft(5),
		Print(match space {
				0 | 3 => "├───┼",
				2 | 5 => "┼───┤",
				1 | 4 => "┼───┼",
				6 => "└───┴",
				7 => "┴───┴",
				8 => "┴───┘",
				_ => "",
			}
		).to_owned(),
		ResetColor,
		
		MoveTo(0,7),
	).unwrap();
}

fn sel_move(board: T3Board, start_piece: u8) -> u8 {
	// clear the screen
	
	let mut selected_spot = start_piece;

	// I don't feel like making a complicated build config system,
	// so if you want to change this, you'll have to change the code right here.
	const ENABLE_LOOPING: bool = true; 

	update_screen_for_selmove(selected_spot, board);
	enable_raw_mode().unwrap();
	loop {
		if let Event::Key(KeyEvent {
			code,
			kind: KeyEventKind::Press,
			..
		}) = read().unwrap() {
			match code {
				KeyCode::Enter => { 
					cleanup_for_selmove(); 
					let mut temp = board;
					temp.make_move(selected_spot);
					update_screen_for_selmove(selected_spot, temp);
					return selected_spot; 
				},
				KeyCode::Char('q') => { 
					cleanup_for_selmove(); 
					exit(0); 
				},

				KeyCode::Up => {
					if selected_spot > 2 {
						selected_spot -= 3;
					} else if ENABLE_LOOPING {
						selected_spot += 6;
					}
					update_screen_for_selmove(selected_spot, board);
				},
				KeyCode::Down => {
					if selected_spot < 6 { 
						selected_spot += 3; 
					} else if ENABLE_LOOPING {
						selected_spot -= 6;
					}
					update_screen_for_selmove(selected_spot, board);
				},
				KeyCode::Left => {
					if [1,2,4,5,7,8].contains(&selected_spot) {
						selected_spot -= 1;
					} else if ENABLE_LOOPING {
						selected_spot += 2;
					}
					update_screen_for_selmove(selected_spot, board);
				},
				KeyCode::Right => {
					if [0,1,3,4,6,7].contains(&selected_spot) {
						selected_spot += 1;
					} else if ENABLE_LOOPING {
						selected_spot -= 2;
					}
					update_screen_for_selmove(selected_spot, board);
				},
				_ => (),
			}
		}
	}
}

fn handle_server(stream: &mut TcpStream) {
	let mut buf: [u8; 512] = [0; BUF_SIZE];

	match stream.read(&mut buf) {
		Ok(bytes_read) => {
			let recvd_bytes = &buf[..bytes_read];
			if recvd_bytes.len() == 0 {
				return;
			}

			let cmd_byte = recvd_bytes[0];
			match CMDS::of(cmd_byte) {
				CMDS::KILL => {
                    log(LogLevel::INFO, "Received kill signal; terminating...");
                    std::process::exit(0);
                },
				CMDS::TEXT => {
					if let Ok(string) = std::str::from_utf8(&recvd_bytes[1..]) {
						println!("{string}");
					}
				},
				_ => ()
			}
		}
		Err(_) => ()
	}
}

fn start_menu

fn main() -> io::Result<()> {
	let args: Vec<String> = env::args().collect();
	println!("Connecting to server at address: {T3ONLINE_CLIENTCONNIP}:{T3ONLINE_SERVERPORT}");
	log(LogLevel::INFO, format!("Sending data '{}' ...",
		if args.len() > 2 { args[2].as_str() } else { "" }).as_str());

	// execute!(
		// io::stdout(),
		// Hide,
	// ).unwrap();
	let mut stream = TcpStream::connect(format!("{T3ONLINE_CLIENTCONNIP}:{T3ONLINE_SERVERPORT}"))?;
	stream.set_nodelay(true).unwrap();

	writestream(&mut stream, b"About to send a board when the user's done", CMDS::TEXT)?;
	let mut board = T3Board::new();
	let mut last_move = 0;
	loop {
		last_move = sel_move(board, last_move);
		board.make_move(last_move);
		let ter = board.encode215();
		println!("ter: {ter}");
		let (winners, _) = board.check_winner();
		if winners.len() == 1 || board.n_p() == 9 {
			println!("winner: {}", if board.n_p() < 9 && winners.len() == 1 { winners[0] } else { 0 });
			break;
		}
	}
	let ter = int2bytes(board.encode215() as u32, 2);
	writestream(&mut stream, ter.as_slice(), CMDS::BOARD)?;
	loop {
		handle_server(&mut stream);
		stream.flush().unwrap();
	}
	// Ok(())
}
