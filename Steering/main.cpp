
#include "main.h"


const int	MAX_NB_BOTS = 10;


cCanvas	*g_CanvasPtr;


int main(void)
{
	// Create canvas and vehicle
	g_CanvasPtr = CreateCanvas(640, 480);

	// Record the VM instance
	iVirtualMachine *vm_ptr = iVirtualMachine::GetInstance();
	vm_ptr->SetInstructionFrame(500);

	// Register the vehicle class
	vm_REGISTER_CLASS(Vehicle, vm_InheritsNone);

	// Load the compiled VM unit
	vm_ptr->LoadUnit("output.vmu");

	// Create all the bots and obtain a pointer to their base classes
	Vehicle	*vehicles[MAX_NB_BOTS];
	for (int i = 0; i < MAX_NB_BOTS; i++)
		vehicles[i] = (Vehicle *)vm_ptr->GetNativeObject(vm_ptr->NewObject("output.Bot"));

	while (!g_CanvasPtr->IsKeyPressed())
	{
		// Process all running VM threads
		vm_ptr->ProcessFrame();

		// Draw the scene
		g_CanvasPtr->BeginFrame();
		g_CanvasPtr->ClearScreen(cColour(0.8f, 0.8f, 0.8f));
		g_CanvasPtr->DrawBresenhamCircleOutline(320, 240, 80, cColour(0.7f, 0.7f, 0.7f), cColour(0, 0, 0));
		for (int j = 0; j < MAX_NB_BOTS; j++) vehicles[j]->Draw();
		g_CanvasPtr->EndFrame();
	}

	DestroyCanvas(g_CanvasPtr);

	return (0);
}