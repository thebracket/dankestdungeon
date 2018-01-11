--
-- Name: adventurers; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE adventurers (
    id bigint NOT NULL,
    username character varying DEFAULT 30 NOT NULL,
    password_hash character varying DEFAULT 64 NOT NULL
);


ALTER TABLE adventurers OWNER TO postgres;

--
-- Name: adventurers_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE adventurers_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE adventurers_id_seq OWNER TO postgres;

--
-- Name: adventurers_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE adventurers_id_seq OWNED BY adventurers.id;


--
-- Name: dungeons; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE dungeons (
    id bigint NOT NULL,
    name character varying(64) NOT NULL,
    level_data text NOT NULL,
    adventurer_id bigint NOT NULL
);


ALTER TABLE dungeons OWNER TO postgres;

--
-- Name: dungeons_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE dungeons_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE dungeons_id_seq OWNER TO postgres;

--
-- Name: dungeons_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE dungeons_id_seq OWNED BY dungeons.id;


--
-- Name: result; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE result (
    id bigint NOT NULL,
    dungeon_id bigint NOT NULL,
    is_win boolean NOT NULL,
    turns integer NOT NULL,
    rating integer NOT NULL,
    adventurer_id bigint NOT NULL
);


ALTER TABLE result OWNER TO postgres;

--
-- Name: result_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE result_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE result_id_seq OWNER TO postgres;

--
-- Name: result_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE result_id_seq OWNED BY result.id;


--
-- Name: tokens; Type: TABLE; Schema: public; Owner: postgres; Tablespace:
--

CREATE TABLE tokens (
    id bigint NOT NULL,
    adventurer_id bigint NOT NULL,
    pointintime timestamp without time zone DEFAULT now()
);


ALTER TABLE tokens OWNER TO postgres;

--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY adventurers ALTER COLUMN id SET DEFAULT nextval('adventurers_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY dungeons ALTER COLUMN id SET DEFAULT nextval('dungeons_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY result ALTER COLUMN id SET DEFAULT nextval('result_id_seq'::regclass);


--
-- Name: adventurers_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace:
--

ALTER TABLE ONLY adventurers
    ADD CONSTRAINT adventurers_pkey PRIMARY KEY (id);


--
-- Name: dungeons_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace:
--

ALTER TABLE ONLY dungeons
    ADD CONSTRAINT dungeons_pkey PRIMARY KEY (id);


--
-- Name: result_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace:
--

ALTER TABLE ONLY result
    ADD CONSTRAINT result_pkey PRIMARY KEY (id);


--
-- Name: tokens_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace:
--

ALTER TABLE ONLY tokens
    ADD CONSTRAINT tokens_pkey PRIMARY KEY (id);


--
-- Name: adventurers_id_uindex; Type: INDEX; Schema: public; Owner: postgres; Tablespace:
--

CREATE UNIQUE INDEX adventurers_id_uindex ON adventurers USING btree (id);


--
-- Name: adventurers_username_uindex; Type: INDEX; Schema: public; Owner: postgres; Tablespace:
--

CREATE UNIQUE INDEX adventurers_username_uindex ON adventurers USING btree (username);


--
-- Name: dungeons_adventurer_id_index; Type: INDEX; Schema: public; Owner: postgres; Tablespace:
--

CREATE INDEX dungeons_adventurer_id_index ON dungeons USING btree (adventurer_id);


--
-- Name: dungeons_id_uindex; Type: INDEX; Schema: public; Owner: postgres; Tablespace:
--

CREATE UNIQUE INDEX dungeons_id_uindex ON dungeons USING btree (id);


--
-- Name: result_adventurer_id_index; Type: INDEX; Schema: public; Owner: postgres; Tablespace:
--

CREATE INDEX result_adventurer_id_index ON result USING btree (adventurer_id);


--
-- Name: result_dungeon_id_index; Type: INDEX; Schema: public; Owner: postgres; Tablespace:
--

CREATE INDEX result_dungeon_id_index ON result USING btree (dungeon_id);


--
-- Name: result_id_uindex; Type: INDEX; Schema: public; Owner: postgres; Tablespace:
--

CREATE UNIQUE INDEX result_id_uindex ON result USING btree (id);


--
-- Name: result_rating_index; Type: INDEX; Schema: public; Owner: postgres; Tablespace:
--

CREATE INDEX result_rating_index ON result USING btree (rating);


--
-- Name: result_turns_index; Type: INDEX; Schema: public; Owner: postgres; Tablespace:
--

CREATE INDEX result_turns_index ON result USING btree (turns);


--
-- Name: tokens_adventurer_id_index; Type: INDEX; Schema: public; Owner: postgres; Tablespace:
--

CREATE INDEX tokens_adventurer_id_index ON tokens USING btree (adventurer_id);


--
-- Name: dungeons_adventurers_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY dungeons
    ADD CONSTRAINT dungeons_adventurers_id_fk FOREIGN KEY (adventurer_id) REFERENCES adventurers(id);


--
-- Name: result_adventurers_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY result
    ADD CONSTRAINT result_adventurers_id_fk FOREIGN KEY (adventurer_id) REFERENCES adventurers(id);


--
-- Name: result_dungeons_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY result
    ADD CONSTRAINT result_dungeons_id_fk FOREIGN KEY (dungeon_id) REFERENCES dungeons(id);


--
-- Name: tokens_adventurers_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY tokens
    ADD CONSTRAINT tokens_adventurers_id_fk FOREIGN KEY (adventurer_id) REFERENCES adventurers(id);

