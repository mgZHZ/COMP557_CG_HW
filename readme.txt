
demo for a1-a3 ➡️ here: https://drive.google.com/drive/folders/1wBPDhdNGI6KogukW1WXjk-0QzKzkZk2v?usp=share_link

sample code from Assignment1 is provided :)

Keyboard Control:  
	Space: pause the motion
	Enter: reverse the motion
	Up: faster
	Down: slower

	1: switch to 0019_AdvanceBollywoodDance001.bvh
	2: switch to Cyrus_Take6.bvh
	3: switch to OptiTrack-IITSEC2007.bvh

	After switching, all the frame speed and motion will be reset.

Basic rendering structure:
	- main()
		- render()
			-  draw_axis(...)
				- draw_axis(...)
				- draw_line(...)
			-  drawBVH(...) : bfs traversal all the DAGnodes
				- doOTR(...): calculate accumulated matrix
				- draw_line(...): connect the joints
				- draw_axis(...): draw an axis over each joint
				- dagnode->draw(...): draw the object that the node corresponding to
	

Issue: the way of line drawing is inefficient, it requires a large amount of memory. The macOS will kill this thread when the use of memory reach to ~4GB. This might due to a super inefficient way of using buffer (such as the buffer never being released...?).

Update： add glDeleteBuffers(1, &posBufID) at line 268, running time is extended from 15s to 55s.

