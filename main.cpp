//A really simple wm that does almost nothing
#include <X11/Xlib.h>
#include <iostream>
#include <vector>
#include <cstdlib>

#define StringToKeysym(a) XKeysymToKeycode(disp, XStringToKeysym(a))
void tile(Display* disp);
class Client
{
	public:
		Window win;
};
class Desk
{
	public:
		std::vector<Client*> clients;
		Client* top;
};
Desk* currentdesk;
void Add_Window(Window w, Desk* desk)
{
	std::cout << "Adding window" << std::endl;
	Client* c;
	c = new Client;
	c->win = w;
	desk->clients.push_back(c);
	std::cout << "Added window" << std::endl;
}

void Remove_Window(Window w)
{
	std::cout << "Removing window" << std::endl;
	for(int i = 0; i < currentdesk->clients.size(); i++)
	{
		if(currentdesk->clients[i]->win == w)
		{
			delete currentdesk->clients[i];
			currentdesk->clients.erase(currentdesk->clients.begin()+i);
		}
	}
	std::cout << "Removed window" << std::endl;
}

void ChangeDesk(Display* disp, Desk* d)
{
	//unmap all the windows in the current desktop
	for(int i = 0; i < currentdesk->clients.size(); i++)
	{
		XUnmapWindow(disp, currentdesk->clients[i]->win);
	}

	currentdesk = d;

	for(int i = 0; i < currentdesk->clients.size(); i++)
	{
		XMapWindow(disp, currentdesk->clients[i]->win);
	}
	tile(disp);
}
//Event bullshit

void ConfigureRequestCB(Display* disp, XEvent* e)
{
	std::cout << "Config window event recieved" << std::endl;
	XConfigureRequestEvent* ev = &e->xconfigurerequest;
	XWindowChanges wc;
	wc.x = ev->x;
	wc.y = ev->y;
	wc.width = ev->width;
	wc.height = ev->height;
	wc.border_width = ev->border_width;
	wc.sibling = ev->above;
	wc.stack_mode = ev->detail;
	XConfigureWindow(disp, ev->window, ev->value_mask, &wc);
}
void MapRequestCB(Display* disp, XEvent* e)
{
	std::cout << "Getting a map event" << std::endl;
	XMapRequestEvent* ev = &e->xmaprequest;
	Add_Window(ev->window, currentdesk);
	XMapWindow(disp, ev->window);
	tile(disp);
}

void DestroyNotifyCB(Display* disp, XEvent* e)
{
	XDestroyWindowEvent* ev = &e->xdestroywindow;
	std::cout << "Destroy event recieved" << std::endl;
	for(int i = 0; i < currentdesk->clients.size(); i++)
	{
		if(ev->window == currentdesk->clients[i]->win)
		{
			Remove_Window(ev->window);
			tile(disp);
			std::cout <<"Destoryed" << std::endl;
			return;
		}
	}
	std::cout << "Couldn't find window" << std::endl;
}
int w,h;
int screen;
int master_size;


void tile(Display* disp)
{
	if(currentdesk->clients.size() == 0)
		return;
	int y = 0;
	//if only one window
	if(currentdesk->clients.size() == 1)
	{
		XMoveResizeWindow(disp, currentdesk->clients[0]->win, 0, 0, w-2, h-2);
		return;
	}
	XMoveResizeWindow(disp, currentdesk->clients[0]->win, 0, 0, master_size-2, h-2);
	for(int i = 1; i < currentdesk->clients.size(); i++)
	{
		XMoveResizeWindow(disp, currentdesk->clients[i]->win, master_size, y, w-master_size-2, (h/(currentdesk->clients.size()-1)-2));
		y += h/(currentdesk->clients.size()-1);
	}
}
int main()
{
	Display* disp;
	Window rootw;
	XWindowAttributes attrb;

	XButtonEvent start;
	XEvent ev;

	//put any keys we want to listen to here
	std::vector<std::string> keys;
	//todo: read keybindings from a file and then load them all. For now, justload some defaults
	keys.push_back("t"); //for raising a terminal
	keys.push_back("e"); //for exiting
	keys.push_back("s"); //for switching desktops
	keys.push_back("r"); //dmenu

	if(!(disp = XOpenDisplay(0x0)))
	{
		std::cout << "Error: display could not be created\n";
	}

	rootw = DefaultRootWindow(disp);
	XSelectInput(disp, rootw, SubstructureNotifyMask|SubstructureRedirectMask);

	screen = DefaultScreen(disp);
	w = XDisplayWidth(disp, screen);
	h = XDisplayHeight(disp, screen);
	master_size = w * 0.5; //master window size
	Desk* d1 = new Desk;
	Desk* d2 = new Desk;
	currentdesk = d1;

	for(int i = 0; i < keys.size(); i++)
	{
		XGrabKey(disp, StringToKeysym(keys[i].c_str()), Mod4Mask, rootw, True, GrabModeAsync, GrabModeAsync);
	}

	//grab the mouse buttons when the mod4 is held down
	XGrabButton(disp, 1, Mod4Mask, rootw, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None);
	XGrabButton(disp, 3, Mod4Mask, rootw, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None);

	bool run = true;
	while(run)
	{
		XNextEvent(disp, &ev);

		//check to see if we pressed t
		if(ev.type == KeyPress && ev.xkey.keycode == StringToKeysym("t"))
		{
			//open xterm
			system("xterm &");
		}
		if(ev.type == KeyPress && ev.xkey.keycode == StringToKeysym("e"))
		{
			run = false;
		}
		if(ev.type == KeyPress && ev.xkey.keycode == StringToKeysym("r"))
		{
			system("dmenu_run");
		}
		if(ev.type == KeyPress && ev.xkey.keycode == StringToKeysym("s"))
		{
			if(currentdesk == d1)
			{
				//switch to d2
				ChangeDesk(disp, d2);
				std::cout << "Changing to d2" << std::endl;
			}
			else
			{
				//switch to d1
				ChangeDesk(disp, d1);
				std::cout << "Changing to d1" << std::endl;
			}
		}
		if(ev.type == ConfigureRequest)
		{
			ConfigureRequestCB(disp, &ev);
		}
		if(ev.type == MapRequest)
		{
			MapRequestCB(disp, &ev);
		}
		if(ev.type == DestroyNotify)
		{
			DestroyNotifyCB(disp, &ev);
		}
		//todo: parse some buttons or something
	}
	//Free everything
	XCloseDisplay(disp);
	return 0;
}