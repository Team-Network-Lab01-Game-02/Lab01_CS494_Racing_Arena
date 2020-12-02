import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.*;

public class CommandLineClient {
    public static void main(String[] args) {
        String hostName = "localhost";
        Integer portNumber = 6969;

        try (
                Socket echoSocket = new Socket(hostName, portNumber);
                PrintWriter out = new PrintWriter(echoSocket.getOutputStream(), true);
                BufferedReader in =
                        new BufferedReader(new InputStreamReader(echoSocket.getInputStream()));
                BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in))
        ) {
            System.out.println("Connected\n");

            in.readLine();
            while (true) {
                System.out.print("Name: ");
                String name = stdIn.readLine();
                System.out.println();

                out.println(name);
                String setNameResult = in.readLine();

                if (setNameResult.equals("username_request_status,Success")) {
                    System.out.println("Name set successfully\n");
                    out.println("ok");
                    break;
                } else {
                    System.out.println("Name already exists, please enter again");
                }
            }

            Map<Integer, String> names = new HashMap<>();

            String usernameIdMapping = in.readLine();
            List<String> usernameIdMappingTokens = Arrays.asList(usernameIdMapping.split(","));
            System.out.println("Players:");
            for (Integer i = 2; i < usernameIdMappingTokens.size() - 1; ++i) {
                System.out.println(usernameIdMappingTokens.get(i));
            }
            System.out.println();
            out.println("ok");

            String scoreInfo = in.readLine();
            List<String> scoreInfoTokens = Arrays.asList(scoreInfo.split(","));

            System.out.println("Start score: " + scoreInfoTokens.get(1));
            System.out.println("End score: " + scoreInfoTokens.get(2));
            System.out.println();
            out.println("ok");

            while (true) {
                in.readLine();
                out.println("ok");

                String question = in.readLine();
                List<String> questionTokens = Arrays.asList(question.split(","));
                System.out.println("Question: " + questionTokens.get(1));
                System.out.print("Answer: ");

                Long startTime = System.currentTimeMillis();
                String answer = stdIn.readLine();
                Long endTime = System.currentTimeMillis();
                System.out.println();
                Long duration = endTime - startTime;
                System.out.println("Duration: " + duration + "\n");

                out.println(answer + "," + duration);

                String answerInfo = in.readLine();
                List<String> answerInfoTokens = Arrays.asList(answerInfo.split(","));

                System.out.println("Correct answer: " + answerInfoTokens.get(1) + "\n");

                if (answerInfoTokens.get(2).equals("Correct")) {
                    System.out.println("You're correct!\n");
                } else {
                    System.out.println("You're wrong\n");
                }

                if (answerInfoTokens.get(3).equals("Fastest")) {
                    System.out.println("You're the fastest player!!!\n");
                }

                List<String> removedPlayers = new ArrayList<>();
                Integer removedPlayersIndex = 6;
                while (!answerInfoTokens.get(removedPlayersIndex).equals("end")) {
                    removedPlayers.add(answerInfoTokens.get(removedPlayersIndex));
                    removedPlayersIndex += 1;
                }

                List<String> scores = new ArrayList<>();
                for (Integer i = removedPlayersIndex + 2; i < answerInfoTokens.size() - 1; ++i) {
                    scores.add(answerInfoTokens.get(i));
                }

                System.out.println("Scores:");
                for (String score : scores) {
                    System.out.println(score);
                }
                System.out.println();

                System.out.println("Removed players:");
                for (String removedPlayer : removedPlayers) {
                    System.out.println(removedPlayer);
                }
                System.out.println();

                if (answerInfoTokens.get(4).equals("Remove")) {
                    System.out.println("You have been removed\n");
                    break;
                }

                out.println("ok");

                /*
                System.out.println("Continue? (y/n)");
                String cont = stdIn.readLine();
                System.out.println();
                out.println(cont);
                if (cont.equals("n")) {
                    break;
                }
                 */

                String cont = in.readLine();
                out.println("ok");
                if (cont.equals("continue,n")) {
                    System.out.println("Game is over");

                    break;
                }
            }
        } catch (IOException e) {
            System.out.println("Exception: " + e.getMessage());
        }
    }
}
