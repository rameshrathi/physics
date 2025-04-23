import Foundation
import nwlib

//-------------------------------
// ----------- Run --------------
//-------------------------------

@main
public struct Run {

	public static func main() {
		start_main(
            CommandLine.argc,
            CommandLine.unsafeArgv
        )
		
		let email = "ramesh@example.com"
		
		if #available(macOS 13.0, *)
		{
			
			let regex = /@(?<domain>[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})$/
			if let match = email.firstMatch(of: regex) {
				print("Match = \(String(match.output.domain))")
			} else {
				print("Error")
			}
		}
		else {
			// Fallback on earlier versions
		}
		
	}
}
