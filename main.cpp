#include <iostream>
#include <Windows.h>
#include <string>
#include <ctime>


std::string game_name = "Tradewinds";
LPCSTR l_game_window = "Tradewinds";
std::string game_status;

bool is_game_avail;
bool update_on_next_run;

//money vals
bool cash_status = false;
BYTE cash_values[] = {0x00f42400, 0x255, 0x255};
DWORD cash_base_address = {0x0465d74};
DWORD cash_offsets[] = {0x10, 0x0};

DWORD find_dm_addr(int pointer_level, HANDLE h_proc_handle, DWORD offsets[], DWORD base_address){
	DWORD pointer = base_address;
	DWORD p_temp;
	DWORD pointer_addr;
	for(int i = 0; i < pointer_level; i++){
		if (i == 0){
			ReadProcessMemory(h_proc_handle, (LPCVOID) pointer, &p_temp, sizeof(p_temp), NULL);
		}
		pointer_addr = p_temp + offsets[i];
		ReadProcessMemory(h_proc_handle, (LPCVOID) pointer_addr, &p_temp, sizeof(p_temp), NULL);
	}
	return pointer_addr;
}


void write_to_memory(HANDLE h_proc_handle){
	if(cash_status){
		DWORD cash_address = find_dm_addr(1, h_proc_handle, cash_offsets, cash_base_address);
		WriteProcessMemory(h_proc_handle, (BYTE*)cash_address, &cash_values, sizeof(cash_values), NULL);
		cash_status = !cash_status;
	}
}

int main(){
	HWND h_game_window = NULL;
	int time_since_last_update = clock();
	int game_avail_timer = clock();
	int one_press_timer = clock();
	DWORD dw_proc_id = NULL;
	HANDLE h_proc_handle = NULL;
	update_on_next_run = true;
	std::string s_cash_status = "OFF";

	while(!GetAsyncKeyState(VK_INSERT)){
		if ((clock() - game_avail_timer) > 100){
			game_avail_timer = clock();
			is_game_avail = false;
			h_game_window = FindWindow(NULL, l_game_window);
			if (h_game_window){
				GetWindowThreadProcessId(h_game_window, &dw_proc_id);
				if (dw_proc_id != 0){
					h_proc_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dw_proc_id);
					if (h_proc_handle == INVALID_HANDLE_VALUE || h_proc_handle == NULL){
						game_status = "Failed to open process for valid handle";
					}
					else{
						game_status = "Tradewinds initialized";
						is_game_avail = true;
					}
				}
				else{
					game_status = "Failed to get process ID";
				}
			}
			else{
				game_status = "Tradewinds not found";
			}
			if (update_on_next_run || clock() - time_since_last_update > 5000){
				system("cls");
				std::cout<<"--------------------------------------------------\n";
				std::cout<<"			Tradewinds trainer\n";
				std::cout<<"--------------------------------------------------\n";
				std::cout<<"Game status: "<< game_status<<std::endl<<std::endl;
				std::cout<<"[F1] Increase cash -> " <<cash_status<<std::endl;
				std::cout<<"[Insert] Exit"<<std::endl;
				update_on_next_run = false;
				time_since_last_update = clock();
			}
			if (is_game_avail){
				write_to_memory(h_proc_handle);
			}
			if(clock() - one_press_timer > 400){
				if (is_game_avail){
					if(GetAsyncKeyState(VK_F1)){
						one_press_timer = clock();
						cash_status = !cash_status;
						update_on_next_run = true;
						//if (cash_status){
							//s_cash_status = "Activated";
						//}
						//else {
							//s_cash_status = "On standby";
						//}
					}
				}

		}
		}
	}
	CloseHandle(h_proc_handle);
	CloseHandle(h_game_window);
	return ERROR_SUCCESS;
}
