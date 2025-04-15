import Testing
import Foundation
import nwlib
import runner

@Suite("nwlib-tests")
struct NwLibTests {
    @Test func test_capture() async throws {
        
        nw_start_process()
    }
}
