import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.*;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

public class Server {
    // server announces, to 1 or all?

    private static final Set<Integer> currentPlayers = new HashSet<>();

    private static final List<BlockingQueue<String>> toThreadQueues = new ArrayList<>();
    private static final List<BlockingQueue<String>> fromThreadQueues = new ArrayList<>();

    private static final List<String> names = new ArrayList<>();

    public static synchronized Integer setName(Integer index, String name) {
        for (String currentName : names) {
            if (currentName.equals(name)) {
                return 0;
            }
        }

        names.set(index, name);

        /*
        try {
            for (Integer i : currentPlayers) {
                toThreadQueues.get(i).put(name);
            }
        } catch (InterruptedException e) {
            System.out.println("Exception: " + e.getMessage());
        }
         */

        return 1;
    }

    public static void main(String[] args) {
        final Integer portNumber = 6969;
        final Integer numPlayers = 3;
        final Integer startScore = 0;
        final Integer endScore = 10;
        final Integer maxConsecWrong = 2;

        try (
                ServerSocket serverSocket = new ServerSocket(portNumber)
        ) {
            for (Integer i = 0; i < numPlayers; ++i) {
                names.add("");
            }

            List<Integer> scores = new ArrayList<>();

            for (Integer i = 0; i < numPlayers; ++i) {
                scores.add(startScore);
            }

            List<Integer> consecWrong = new ArrayList<>();

            for (Integer i = 0; i < numPlayers; ++i) {
                consecWrong.add(0);
            }

            for (Integer i = 0; i < numPlayers; ++i) {
                toThreadQueues.add(new ArrayBlockingQueue<>(256));
                fromThreadQueues.add(new ArrayBlockingQueue<>(256));
            }

            List<Thread> playerThreads = new ArrayList<>();

            System.out.println("Listening");

            for (Integer i = 0; i < numPlayers; ++i) {
                Socket clientSocket = serverSocket.accept();
                PlayerRunnable playerRunnable = new PlayerRunnable(
                        clientSocket, fromThreadQueues.get(i), toThreadQueues.get(i), i);
                Thread playerThread = new Thread(playerRunnable);

                playerThreads.add(playerThread);

                currentPlayers.add(i);

                playerThread.start();
            }

            for (Integer i = 0; i < numPlayers; ++i) {
                fromThreadQueues.get(i).take();
            }

            String usernameIdMapping = "username_id_mapping,start";
            for (Integer i = 0; i < numPlayers; ++i) {
                usernameIdMapping += "," + i + " " + names.get(i);
            }
            usernameIdMapping += ",end";
            for (Integer i = 0; i < numPlayers; ++i) {
                toThreadQueues.get(i).put(usernameIdMapping);
            }

            String scoreInfo = "score_info," + startScore + "," + endScore;

            for (Integer i = 0; i < numPlayers; ++i) {
                toThreadQueues.get(i).put(scoreInfo);
            }

            Random rand = new Random();

            while (true) {
                Integer num1 = rand.nextInt(10);
                Integer num2 = rand.nextInt(10);

                Integer correctAnswer = num1 + num2;

                String question = "question," + num1 + " + " + num2;

                for (Integer i : currentPlayers) {
                    toThreadQueues.get(i).put(question);
                }

                List<Boolean> correct = new ArrayList<>();

                Integer fastestIndex = -1;
                Long fastestDuration = Long.MAX_VALUE;

                Integer loseCount = 0;

                List<Integer> removedPlayers = new ArrayList<>();

                for (Integer i : currentPlayers) {
                    Integer answer = Integer.parseInt(fromThreadQueues.get(i).take());
                    Long duration = Long.parseLong(fromThreadQueues.get(i).take());

                    if (answer.equals(correctAnswer)) {
                        correct.add(true);

                        Integer currentScore = scores.get(i);
                        scores.set(i, currentScore + 1);

                        consecWrong.set(i, 0);
                    } else {
                        correct.add(false);

                        Integer currentScore = scores.get(i);
                        scores.set(i, currentScore - 1);

                        if (scores.get(i) < 0) {
                            scores.set(i, 0);
                        }

                        loseCount += 1;

                        Integer curConsecWrong = consecWrong.get(i);
                        consecWrong.set(i, curConsecWrong + 1);

                        if (consecWrong.get(i) > maxConsecWrong) {
                            removedPlayers.add(i);
                        }
                    }

                    if (duration < fastestDuration) {
                        fastestIndex = i;
                        fastestDuration = duration;
                    }
                }

                Integer currentScoreFastest = scores.get(fastestIndex);
                scores.set(fastestIndex, currentScoreFastest + loseCount - 1);

                for (Integer i : currentPlayers) {
                    String answerInfo = "answer_info,";

                    answerInfo += correctAnswer;

                    if (correct.get(i)) {
                        answerInfo += ",Correct";
                    } else {
                        answerInfo += ",Wrong";
                    }

                    if (i.equals(fastestIndex)) {
                        answerInfo += ",Fastest";
                    } else {
                        answerInfo += ",Not fastest";
                    }

                    boolean remain = true;

                    for (Integer j : removedPlayers) {
                        if (j.equals(i)) {
                            remain = false;
                            break;
                        }
                    }

                    if (remain) {
                        answerInfo += ",Remain";
                    } else {
                        answerInfo += ",Remove";
                    }

                    answerInfo += ",start";
                    for (Integer j : removedPlayers) {
                        answerInfo += "," + j;
                    }
                    answerInfo += ",end";

                    answerInfo += ",start";
                    for (Integer j : currentPlayers) {
                        answerInfo += "," + j + " " + scores.get(j);
                    }
                    answerInfo += ",end";

                    toThreadQueues.get(i).put(answerInfo);
                }

                boolean cont = true;

                /*
                for (Integer i = 0; i < numPlayers; ++i) {
                    String contResponse = fromThreadQueues.get(i).take();

                    if (contResponse.equals("n")) {
                        cont = false;
                    }
                }
                 */

                for (Integer i : currentPlayers) {
                    if (scores.get(i) >= endScore) {
                        cont = false;
                        break;
                    }
                }

                if (cont) {
                    for (Integer i : currentPlayers) {
                        toThreadQueues.get(i).put("continue,y");
                    }
                } else {
                    for (Integer i : currentPlayers) {
                        toThreadQueues.get(i).put("continue,n");
                    }
                    break;
                }

                for (Integer i : removedPlayers) {
                    currentPlayers.remove(i);
                }
            }

            for (Integer i = 0; i < numPlayers; ++i) {
                playerThreads.get(i).join();
            }
        } catch (IOException | InterruptedException e) {
            System.out.println("Exception: " + e.getMessage());
        }
    }
}
