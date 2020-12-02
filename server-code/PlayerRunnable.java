import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.BlockingQueue;

public class PlayerRunnable implements Runnable {
    // handle exceptions
    // duplicates

    private final Socket clientSocket;

    private final BlockingQueue<String> toCenterQueue;
    private final BlockingQueue<String> fromCenterQueue;

    private final Integer index;

    PlayerRunnable(Socket clientSocket,
                   BlockingQueue<String> toCenterQueue,
                   BlockingQueue<String> fromCenterQueue,
                   Integer index) {
        this.clientSocket = clientSocket;
        this.toCenterQueue = toCenterQueue;
        this.fromCenterQueue = fromCenterQueue;
        this.index = index;
    }

    public void run() {
        System.out.println("Thread " + index + " started");

        try (
                PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);
                BufferedReader in =
                        new BufferedReader(new InputStreamReader(clientSocket.getInputStream()))
        ) {
            out.println("username_request");

            while (true) {
                String name = in.readLine();
                Integer setNameResult = Server.setName(index, name);

                if (setNameResult == 1) {
                    out.println("username_request_status,Success");
                    toCenterQueue.put("Name set");

                    break;
                } else {
                    out.println("username_request_status,Failure");
                }
            }

            String usernameIdMapping = fromCenterQueue.take();
            out.println(usernameIdMapping);

            String scoreInfo = fromCenterQueue.take();
            out.println(scoreInfo);

            while (true) {
                String waitReady = fromCenterQueue.take();
                out.println(waitReady);

                String question = fromCenterQueue.take();
                out.println(question);

                String answerTime = in.readLine();
                List<String> tokens = Arrays.asList(answerTime.split(","));
                Integer answer = Integer.parseInt(tokens.get(0));
                Long duration = Long.parseLong(tokens.get(1));

                toCenterQueue.put(answer.toString());
                toCenterQueue.put(duration.toString());

                String answerInfo = fromCenterQueue.take();
                out.println(answerInfo);

                /*
                String cont = in.readLine();
                toCenterQueue.put(cont);
                if (cont.equals("n")) {
                    break;
                }
                */

                String cont = fromCenterQueue.take();
                out.println(cont);

                if (cont.equals("continue,n")) {
                    break;
                }
            }

            clientSocket.close();
        } catch (IOException | InterruptedException e) {
            System.out.println("Exception: " + e.getMessage());
        }
    }
}
